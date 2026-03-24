#include "EnvironmentControlSystem.h"

#include "EngineUtils.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UEnvironmentControlSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<ADirectionalLight> It(World); It; ++It) { CachedSun = *It; break; }
    for (TActorIterator<ASkyLight> It(World); It; ++It) { CachedSkyLight = *It; break; }
    for (TActorIterator<AExponentialHeightFog> It(World); It; ++It) { CachedFog = *It; break; }

    UEnvironmentPreset* Boot = SunnyDay ? SunnyDay : CloudyAfternoon;
    if (Boot)
    {
        ForceApplyPreset(Boot);
    }
    else
    {
        FromSnapshot = CaptureSnapshot();
        ToSnapshot = FromSnapshot;
        BlendAlpha = 1.0f;
    }

    bInitialized = true;
}

void UEnvironmentControlSystem::Tick(float DeltaTime)
{
    if (!bInitialized || BlendAlpha >= 1.0f) return;

    BlendAlpha = FMath::Clamp(BlendAlpha + DeltaTime / FMath::Max(0.01f, TransitionDuration), 0.0f, 1.0f);
    ApplySnapshot(LerpSnapshot(FromSnapshot, ToSnapshot, BlendAlpha));
}

TStatId UEnvironmentControlSystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UEnvironmentControlSystem, STATGROUP_Tickables);
}

void UEnvironmentControlSystem::SetAutoEnvironmentMode(bool bEnabled)
{
    bAutoEnvironmentMode = bEnabled;
}

void UEnvironmentControlSystem::TransitionToPreset(UEnvironmentPreset* Preset, float DurationSeconds)
{
    if (!Preset) return;

    TransitionDuration = FMath::Clamp(DurationSeconds, 10.0f, 20.0f);
    FromSnapshot = CaptureSnapshot();
    ToSnapshot = SnapshotFromPreset(Preset);
    BlendAlpha = 0.0f;
    OnPresetApplied.Broadcast(Preset);
}

void UEnvironmentControlSystem::ForceApplyPreset(UEnvironmentPreset* Preset)
{
    if (!Preset) return;

    FEnvironmentSnapshot Snap = SnapshotFromPreset(Preset);
    FromSnapshot = Snap;
    ToSnapshot = Snap;
    BlendAlpha = 1.0f;
    ApplySnapshot(Snap);
    OnPresetApplied.Broadcast(Preset);
}

void UEnvironmentControlSystem::SetWeatherType(EWeatherType WeatherType)
{
    bAutoEnvironmentMode = false;
    if (UEnvironmentPreset* Preset = GetPresetForWeather(WeatherType))
    {
        TransitionToPreset(Preset, TransitionDuration);
    }
}

void UEnvironmentControlSystem::SetFogDensity(float InFogDensity)
{
    bAutoEnvironmentMode = false;
    FEnvironmentSnapshot Target = CaptureSnapshot();
    Target.FogDensity = FMath::Clamp(InFogDensity, 0.0f, 0.08f);
    FromSnapshot = CaptureSnapshot();
    ToSnapshot = Target;
    BlendAlpha = 0.0f;
}

void UEnvironmentControlSystem::SetTimeOfDay(float InTimeOfDay)
{
    bAutoEnvironmentMode = false;
    FEnvironmentSnapshot Target = CaptureSnapshot();
    Target.TimeOfDay = FMath::Fmod(FMath::Max(0.0f, InTimeOfDay), 24.0f);
    FromSnapshot = CaptureSnapshot();
    ToSnapshot = Target;
    BlendAlpha = 0.0f;
}

void UEnvironmentControlSystem::SetCloudLevel(float InCloudDensity)
{
    bAutoEnvironmentMode = false;
    FEnvironmentSnapshot Target = CaptureSnapshot();
    Target.CloudDensity = FMath::Clamp(InCloudDensity, 0.0f, 1.0f);
    FromSnapshot = CaptureSnapshot();
    ToSnapshot = Target;
    BlendAlpha = 0.0f;
}

void UEnvironmentControlSystem::SetRainIntensity(float InRainIntensity)
{
    bAutoEnvironmentMode = false;
    FEnvironmentSnapshot Target = CaptureSnapshot();
    Target.RainIntensity = FMath::Clamp(InRainIntensity, 0.0f, 1.0f);
    Target.WeatherType = Target.RainIntensity < 0.2f ? EWeatherType::Clear : (Target.RainIntensity < 0.55f ? EWeatherType::LightRain : EWeatherType::HeavyRain);
    FromSnapshot = CaptureSnapshot();
    ToSnapshot = Target;
    BlendAlpha = 0.0f;
}

UEnvironmentPreset* UEnvironmentControlSystem::GetPresetForWeather(EWeatherType WeatherType) const
{
    switch (WeatherType)
    {
        case EWeatherType::Clear: return SunnyDay;
        case EWeatherType::Cloudy: return CloudyAfternoon;
        case EWeatherType::LightRain: return RainyEvening;
        case EWeatherType::HeavyRain: return Storm;
        case EWeatherType::Foggy: return FoggyMorning;
        default: return SunnyDay;
    }
}

void UEnvironmentControlSystem::ApplySnapshot(const FEnvironmentSnapshot& Snapshot)
{
    if (CachedFog.IsValid())
    {
        CachedFog->GetComponent()->SetFogDensity(Snapshot.FogDensity);
        CachedFog->GetComponent()->SetFogInscatteringColor(Snapshot.FogColor);
    }

    if (CachedSun.IsValid())
    {
        CachedSun->GetComponent()->SetIntensity(Snapshot.SunIntensity);
        CachedSun->GetComponent()->SetLightColor(Snapshot.AmbientColor.ToFColor(true));
    }

    if (CachedSkyLight.IsValid())
    {
        CachedSkyLight->GetLightComponent()->SetIntensity(FMath::Lerp(0.4f, 1.2f, Snapshot.CloudDensity));
        CachedSkyLight->GetLightComponent()->SetLightColor(Snapshot.AmbientColor.ToFColor(true));
        CachedSkyLight->GetLightComponent()->RecaptureSky();
    }

    if (Snapshot.SkyboxMaterial)
    {
        UMaterialInterface* SkyMat = Snapshot.SkyboxMaterial.LoadSynchronous();
        if (SkyMat)
        {
            UWorld* World = GetWorld();
            if (World)
            {
                AActor* SkySphere = UGameplayStatics::GetActorOfClass(World, AActor::StaticClass());
                (void)SkySphere; // kept as integration hook for project-specific sky actor.
            }
        }
    }
}

FEnvironmentSnapshot UEnvironmentControlSystem::CaptureSnapshot() const
{
    FEnvironmentSnapshot Snapshot;
    if (CachedFog.IsValid())
    {
        Snapshot.FogDensity = CachedFog->GetComponent()->FogDensity;
        Snapshot.FogColor = CachedFog->GetComponent()->FogInscatteringColor;
    }
    if (CachedSun.IsValid())
    {
        Snapshot.SunIntensity = CachedSun->GetComponent()->Intensity;
    }
    return Snapshot;
}

FEnvironmentSnapshot UEnvironmentControlSystem::SnapshotFromPreset(const UEnvironmentPreset* Preset) const
{
    FEnvironmentSnapshot Out;
    Out.WeatherType = Preset->WeatherType;
    Out.FogDensity = Preset->FogDensity;
    Out.FogColor = Preset->FogColor;
    Out.CloudDensity = Preset->CloudDensity;
    Out.SunIntensity = Preset->SunIntensity;
    Out.AmbientColor = Preset->AmbientColor;
    Out.RainIntensity = Preset->RainIntensity;
    Out.WindStrength = Preset->WindStrength;
    Out.TimeOfDay = Preset->TimeOfDay;
    Out.SkyboxMaterial = Preset->SkyboxMaterial.LoadSynchronous();
    return Out;
}

FEnvironmentSnapshot UEnvironmentControlSystem::LerpSnapshot(const FEnvironmentSnapshot& A, const FEnvironmentSnapshot& B, float Alpha)
{
    FEnvironmentSnapshot Out;
    Out.WeatherType = Alpha < 0.5f ? A.WeatherType : B.WeatherType;
    Out.FogDensity = FMath::Lerp(A.FogDensity, B.FogDensity, Alpha);
    Out.FogColor = FMath::Lerp(A.FogColor, B.FogColor, Alpha);
    Out.CloudDensity = FMath::Lerp(A.CloudDensity, B.CloudDensity, Alpha);
    Out.SunIntensity = FMath::Lerp(A.SunIntensity, B.SunIntensity, Alpha);
    Out.AmbientColor = FMath::Lerp(A.AmbientColor, B.AmbientColor, Alpha);
    Out.RainIntensity = FMath::Lerp(A.RainIntensity, B.RainIntensity, Alpha);
    Out.WindStrength = FMath::Lerp(A.WindStrength, B.WindStrength, Alpha);
    Out.TimeOfDay = FMath::Lerp(A.TimeOfDay, B.TimeOfDay, Alpha);
    Out.SkyboxMaterial = Alpha < 0.5f ? A.SkyboxMaterial : B.SkyboxMaterial;
    return Out;
}
