#pragma once

#include "CoreMinimal.h"
#include "VehicleStatusData.generated.h"

USTRUCT(BlueprintType)
struct FVehicleStatusData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Vehicle|Status")
    float SpeedKmh = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="Vehicle|Status")
    float EngineRPM = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="Vehicle|Status")
    float FuelNormalized = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category="Vehicle|Status")
    float EngineConditionNormalized = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category="Vehicle|Status")
    bool bEngineRunning = true;
};
