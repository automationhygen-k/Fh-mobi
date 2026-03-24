#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VehicleInventory.h"
#include "GarageVehicleSpawner.generated.h"

class APlayerVehicle;
class APlayerController;

UCLASS(BlueprintType)
class UGarageVehicleSpawner : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Garage|Spawn", meta=(WorldContext="WorldContextObject"))
    APlayerVehicle* SpawnSelectedVehicle(UObject* WorldContextObject, APlayerController* PlayerController, const FOwnedVehicleRecord& VehicleRecord, const FTransform& SpawnTransform);

private:
    void ApplySavedState(APlayerVehicle* SpawnedVehicle, const FOwnedVehicleRecord& VehicleRecord) const;
};
