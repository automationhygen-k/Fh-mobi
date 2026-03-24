#include "RegionEnvironmentController.h"

#include "EnvironmentControlSystem.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

URegionEnvironmentController::URegionEnvironmentController()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void URegionEnvironmentController::BeginPlay()
{
    Super::BeginPlay();

    if (!PlayerActor)
    {
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            PlayerActor = PC->GetPawn();
        }
    }

    EnvironmentSystem = GetWorld()->GetSubsystem<UEnvironmentControlSystem>();
}

void URegionEnvironmentController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!PlayerActor || !EnvironmentSystem || !EnvironmentSystem->IsAutoEnvironmentModeEnabled()) return;

    TimeAccumulator += DeltaTime;
    if (TimeAccumulator < SampleIntervalSeconds) return;
    TimeAccumulator = 0.0f;

    EEnvironmentRegion Detected;
    if (!TryDetectRegion(PlayerActor->GetActorLocation(), Detected)) return;
    if (Detected == CurrentRegion) return;

    CurrentRegion = Detected;
    if (UEnvironmentPreset* Preset = GetPresetForRegion(Detected))
    {
        EnvironmentSystem->TransitionToPreset(Preset, TransitionDurationSeconds);
    }
}

bool URegionEnvironmentController::TryDetectRegion(const FVector& WorldPos, EEnvironmentRegion& OutRegion) const
{
    OutRegion = EEnvironmentRegion::Unknown;

    if (RegionProbe)
    {
        FRegionSample Sample;
        if (IRegionProbeProvider::Execute_SampleRegionAt(RegionProbe.GetObject(), WorldPos, Sample))
        {
            OutRegion = Classify(Sample);
            return true;
        }
    }

    // Fallback heuristic when no procedural probe is bound.
    OutRegion = WorldPos.Z > 340.0f ? EEnvironmentRegion::Mountain : EEnvironmentRegion::Forest;
    return true;
}

EEnvironmentRegion URegionEnvironmentController::Classify(const FRegionSample& Sample)
{
    const FString Biome = Sample.BiomeType.ToLower();

    if (Sample.DistanceFromWater < 120.0f || Biome.Contains(TEXT("coast"))) return EEnvironmentRegion::Coastal;
    if (Sample.TerrainHeight > 340.0f || Biome.Contains(TEXT("mountain")) || Biome.Contains(TEXT("rocky"))) return EEnvironmentRegion::Mountain;
    if (Biome.Contains(TEXT("desert")) || (Sample.VegetationDensity < 0.2f && Sample.DistanceFromWater > 260.0f)) return EEnvironmentRegion::Desert;
    if (Biome.Contains(TEXT("forest")) || Sample.VegetationDensity > 0.55f) return EEnvironmentRegion::Forest;

    return EEnvironmentRegion::Unknown;
}

UEnvironmentPreset* URegionEnvironmentController::GetPresetForRegion(EEnvironmentRegion Region) const
{
    switch (Region)
    {
        case EEnvironmentRegion::Mountain: return MountainPreset ? MountainPreset : FallbackPreset;
        case EEnvironmentRegion::Desert: return DesertPreset ? DesertPreset : FallbackPreset;
        case EEnvironmentRegion::Forest: return ForestPreset ? ForestPreset : FallbackPreset;
        case EEnvironmentRegion::Coastal: return CoastalPreset ? CoastalPreset : FallbackPreset;
        default: return FallbackPreset;
    }
}
