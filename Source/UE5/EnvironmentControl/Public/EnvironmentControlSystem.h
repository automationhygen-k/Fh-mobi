#pragma once

#include "CoreMinimal.h"
#include "Subsystems/TickableWorldSubsystem.h"
#include "EnvironmentPreset.h"
#include "EnvironmentControlSystem.generated.h"

class ADirectionalLight;
class ASkyLight;
class AExponentialHeightFog;
class UMaterialInstanceDynamic;

USTRUCT(BlueprintType)
struct FEnvironmentSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) EWeatherType WeatherType = EWeatherType::Clear;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float FogDensity = 0.004f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor FogColor = FLinearColor::White;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float CloudDensity = 0.2f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunIntensity = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor AmbientColor = FLinearColor::White;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float RainIntensity = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float WindStrength = 0.2f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float TimeOfDay = 12.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UMaterialInterface> SkyboxMaterial = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnvironmentPresetApplied, const UEnvironmentPreset*, Preset);

UCLASS(BlueprintType)
class UEnvironmentControlSystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    virtual bool IsTickable() const override { return true; }

    UFUNCTION(BlueprintCallable, Category="Environment")
    void SetAutoEnvironmentMode(bool bEnabled);

    UFUNCTION(BlueprintPure, Category="Environment")
    bool IsAutoEnvironmentModeEnabled() const { return bAutoEnvironmentMode; }

    UFUNCTION(BlueprintCallable, Category="Environment")
    void TransitionToPreset(UEnvironmentPreset* Preset, float DurationSeconds = 12.0f);

    UFUNCTION(BlueprintCallable, Category="Environment")
    void ForceApplyPreset(UEnvironmentPreset* Preset);

    UFUNCTION(BlueprintCallable, Category="Environment|Manual")
    void SetWeatherType(EWeatherType WeatherType);

    UFUNCTION(BlueprintCallable, Category="Environment|Manual")
    void SetFogDensity(float InFogDensity);

    UFUNCTION(BlueprintCallable, Category="Environment|Manual")
    void SetTimeOfDay(float InTimeOfDay);

    UFUNCTION(BlueprintCallable, Category="Environment|Manual")
    void SetCloudLevel(float InCloudDensity);

    UFUNCTION(BlueprintCallable, Category="Environment|Manual")
    void SetRainIntensity(float InRainIntensity);

    UFUNCTION(BlueprintPure, Category="Environment")
    UEnvironmentPreset* GetPresetForWeather(EWeatherType WeatherType) const;

    UPROPERTY(BlueprintAssignable)
    FOnEnvironmentPresetApplied OnPresetApplied;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment|Presets")
    TObjectPtr<UEnvironmentPreset> SunnyDay;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment|Presets")
    TObjectPtr<UEnvironmentPreset> CloudyAfternoon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment|Presets")
    TObjectPtr<UEnvironmentPreset> RainyEvening;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment|Presets")
    TObjectPtr<UEnvironmentPreset> FoggyMorning;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment|Presets")
    TObjectPtr<UEnvironmentPreset> Storm;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment|Runtime", meta=(ClampMin="10.0", ClampMax="20.0"))
    float TransitionDuration = 12.0f;

private:
    void ApplySnapshot(const FEnvironmentSnapshot& Snapshot);
    FEnvironmentSnapshot CaptureSnapshot() const;
    FEnvironmentSnapshot SnapshotFromPreset(const UEnvironmentPreset* Preset) const;
    static FEnvironmentSnapshot LerpSnapshot(const FEnvironmentSnapshot& A, const FEnvironmentSnapshot& B, float Alpha);

private:
    bool bInitialized = false;
    bool bAutoEnvironmentMode = true;
    float BlendAlpha = 1.0f;
    FEnvironmentSnapshot FromSnapshot;
    FEnvironmentSnapshot ToSnapshot;

    TWeakObjectPtr<ADirectionalLight> CachedSun;
    TWeakObjectPtr<ASkyLight> CachedSkyLight;
    TWeakObjectPtr<AExponentialHeightFog> CachedFog;
};
