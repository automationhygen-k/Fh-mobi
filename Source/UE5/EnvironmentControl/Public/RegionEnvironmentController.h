#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnvironmentPreset.h"
#include "RegionEnvironmentController.generated.h"

class UEnvironmentControlSystem;

UENUM(BlueprintType)
enum class EEnvironmentRegion : uint8
{
    Unknown,
    Mountain,
    Desert,
    Forest,
    Coastal
};

USTRUCT(BlueprintType)
struct FRegionSample
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) float TerrainHeight = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString BiomeType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float DistanceFromWater = 1000.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float VegetationDensity = 0.0f;
};

UINTERFACE(BlueprintType)
class URegionProbeProvider : public UInterface
{
    GENERATED_BODY()
};

class IRegionProbeProvider
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Region")
    bool SampleRegionAt(const FVector& WorldPosition, FRegionSample& OutSample) const;
};

UCLASS(ClassGroup=(Environment), meta=(BlueprintSpawnableComponent))
class URegionEnvironmentController : public UActorComponent
{
    GENERATED_BODY()

public:
    URegionEnvironmentController();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    bool TryDetectRegion(const FVector& WorldPos, EEnvironmentRegion& OutRegion) const;
    static EEnvironmentRegion Classify(const FRegionSample& Sample);
    UEnvironmentPreset* GetPresetForRegion(EEnvironmentRegion Region) const;

private:
    UPROPERTY(EditAnywhere, Category="Dependencies")
    TObjectPtr<AActor> PlayerActor;

    UPROPERTY(EditAnywhere, Category="Dependencies")
    TScriptInterface<IRegionProbeProvider> RegionProbe;

    UPROPERTY(EditAnywhere, Category="Region Presets")
    TObjectPtr<UEnvironmentPreset> MountainPreset;

    UPROPERTY(EditAnywhere, Category="Region Presets")
    TObjectPtr<UEnvironmentPreset> DesertPreset;

    UPROPERTY(EditAnywhere, Category="Region Presets")
    TObjectPtr<UEnvironmentPreset> ForestPreset;

    UPROPERTY(EditAnywhere, Category="Region Presets")
    TObjectPtr<UEnvironmentPreset> CoastalPreset;

    UPROPERTY(EditAnywhere, Category="Region Presets")
    TObjectPtr<UEnvironmentPreset> FallbackPreset;

    UPROPERTY(EditAnywhere, Category="Performance", meta=(ClampMin="0.2", ClampMax="2.5"))
    float SampleIntervalSeconds = 0.8f;

    UPROPERTY(EditAnywhere, Category="Transitions", meta=(ClampMin="10.0", ClampMax="20.0"))
    float TransitionDurationSeconds = 12.0f;

    float TimeAccumulator = 0.0f;
    EEnvironmentRegion CurrentRegion = EEnvironmentRegion::Unknown;
    TObjectPtr<UEnvironmentControlSystem> EnvironmentSystem = nullptr;
};
