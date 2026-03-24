#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VehicleDamageComponent.h"
#include "VehicleInventory.generated.h"

USTRUCT(BlueprintType)
struct FOwnedVehicleRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Garage")
    FName VehicleId = TEXT("default");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Garage")
    TSoftClassPtr<APawn> VehicleClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Garage", meta=(ClampMin="0.0", ClampMax="1.0"))
    float FuelNormalized = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Garage")
    FVehicleDamageState DamageState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Garage")
    FString CustomizationJson;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Garage")
    FDateTime LastUsedTime = FDateTime::UtcNow();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVehicleInventoryChanged);

UCLASS(BlueprintType)
class UVehicleInventory : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Garage|Inventory")
    bool AddVehicle(const FOwnedVehicleRecord& VehicleRecord);

    UFUNCTION(BlueprintCallable, Category="Garage|Inventory")
    bool RemoveVehicleById(FName VehicleId);

    UFUNCTION(BlueprintCallable, Category="Garage|Inventory")
    bool UpsertVehicle(const FOwnedVehicleRecord& VehicleRecord);

    UFUNCTION(BlueprintPure, Category="Garage|Inventory")
    bool HasVehicle(FName VehicleId) const;

    UFUNCTION(BlueprintPure, Category="Garage|Inventory")
    bool GetVehicleById(FName VehicleId, FOwnedVehicleRecord& OutVehicleRecord) const;

    UFUNCTION(BlueprintPure, Category="Garage|Inventory")
    const TArray<FOwnedVehicleRecord>& GetAllVehicles() const { return OwnedVehicles; }

    UPROPERTY(BlueprintAssignable, Category="Garage|Inventory")
    FOnVehicleInventoryChanged OnInventoryChanged;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Garage|Inventory", meta=(AllowPrivateAccess="true"))
    TArray<FOwnedVehicleRecord> OwnedVehicles;
};
