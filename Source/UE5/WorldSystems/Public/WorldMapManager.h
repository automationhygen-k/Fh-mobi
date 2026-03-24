#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MapMarker.h"
#include "WorldMapManager.generated.h"

USTRUCT(BlueprintType)
struct FMapLocationData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
    FName MarkerId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
    EMapMarkerType MarkerType = EMapMarkerType::Settlement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
    FVector WorldLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Map")
    bool bDiscovered = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapLocationDiscovered, const FMapLocationData&, LocationData);

UCLASS(BlueprintType)
class UWorldMapManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Map")
    void RegisterOrUpdateMarker(FName MarkerId, EMapMarkerType MarkerType, FVector WorldLocation, bool bDiscovered);

    UFUNCTION(BlueprintCallable, Category="Map")
    bool SetMarkerDiscovered(FName MarkerId, bool bDiscovered = true);

    UFUNCTION(BlueprintCallable, Category="Map")
    bool RemoveMarker(FName MarkerId);

    UFUNCTION(BlueprintPure, Category="Map")
    bool GetMarker(FName MarkerId, FMapLocationData& OutMarkerData) const;

    UFUNCTION(BlueprintPure, Category="Map")
    TArray<FMapLocationData> GetAllMarkers(bool bOnlyDiscovered = false) const;

    UFUNCTION(BlueprintCallable, Category="Map")
    void UpdatePlayerLocation(FVector PlayerLocation);

    UFUNCTION(BlueprintPure, Category="Map")
    FVector GetPlayerLocation() const { return PlayerLocation; }

    UPROPERTY(BlueprintAssignable, Category="Map")
    FOnMapLocationDiscovered OnLocationDiscovered;

private:
    UPROPERTY()
    TMap<FName, FMapLocationData> MarkerMap;

    UPROPERTY()
    FVector PlayerLocation = FVector::ZeroVector;
};
