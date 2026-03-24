#pragma once

#include "CoreMinimal.h"
#include "Subsystems/TickableWorldSubsystem.h"
#include "MapMarker.h"
#include "DiscoveryManager.generated.h"

class UWorldMapManager;

USTRUCT(BlueprintType)
struct FDiscoverableEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discovery")
    FName MarkerId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discovery")
    EMapMarkerType MarkerType = EMapMarkerType::Settlement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discovery")
    FVector WorldLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discovery")
    float DiscoveryRadius = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discovery")
    bool bDiscovered = false;
};

UCLASS(BlueprintType)
class UDiscoveryManager : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;

    UFUNCTION(BlueprintCallable, Category="Discovery")
    void RegisterDiscoverable(const FDiscoverableEntry& Entry);

    UFUNCTION(BlueprintCallable, Category="Discovery")
    bool MarkDiscovered(FName MarkerId);

private:
    void RefreshPlayerLocation();

private:
    UPROPERTY()
    TArray<FDiscoverableEntry> Discoverables;

    UPROPERTY()
    TObjectPtr<UWorldMapManager> WorldMapManager;

    float UpdateAccumulator = 0.0f;
    float UpdateInterval = 0.5f;
    FVector CachedPlayerLocation = FVector::ZeroVector;
};
