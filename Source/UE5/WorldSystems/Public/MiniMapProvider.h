#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MapMarker.h"
#include "WorldMapManager.h"
#include "MiniMapProvider.generated.h"

class UWorldMapManager;

USTRUCT(BlueprintType)
struct FMiniMapSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap")
    FVector PlayerLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap")
    FRotator PlayerRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap")
    TArray<FMapLocationData> NearbyMarkers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap")
    TArray<FVector> NearbyRoadPoints;
};

UINTERFACE(BlueprintType)
class UMiniMapRoadProvider : public UInterface
{
    GENERATED_BODY()
};

class IMiniMapRoadProvider
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Minimap")
    bool GetRoadPointsAround(const FVector& Center, float Radius, TArray<FVector>& OutRoadPoints) const;
};

UCLASS(BlueprintType)
class UMiniMapProvider : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Minimap")
    FMiniMapSnapshot BuildSnapshot(const FVector& PlayerLocation, const FRotator& PlayerRotation, float Radius, const TScriptInterface<IMiniMapRoadProvider>& RoadProvider);

private:
    UPROPERTY()
    TObjectPtr<UWorldMapManager> WorldMapManager;
};
