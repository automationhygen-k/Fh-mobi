#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VehicleRepairService.generated.h"

class APlayerVehicle;

UCLASS(BlueprintType)
class UVehicleRepairService : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Garage|Repair")
    bool RepairVehicle(APlayerVehicle* Vehicle) const;
};
