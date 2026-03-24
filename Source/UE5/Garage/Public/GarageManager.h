#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VehicleInventory.h"
#include "GarageManager.generated.h"

class USaveGameManager;
class APlayerVehicle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGarageVehicleSelected, FName, VehicleId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGarageStateChanged, bool, bInGarage);

UCLASS(BlueprintType)
class UGarageManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Garage")
    void RegisterIntroDeliveredVehicle(TSoftClassPtr<APawn> VehicleClass, FName VehicleId = TEXT("default"));

    UFUNCTION(BlueprintCallable, Category="Garage")
    bool SelectVehicle(FName VehicleId);

    UFUNCTION(BlueprintPure, Category="Garage")
    FName GetSelectedVehicleId() const { return SelectedVehicleId; }

    UFUNCTION(BlueprintPure, Category="Garage")
    UVehicleInventory* GetInventory() const { return Inventory; }

    UFUNCTION(BlueprintCallable, Category="Garage")
    void EnterGarage();

    UFUNCTION(BlueprintCallable, Category="Garage")
    void ExitGarage();

    UFUNCTION(BlueprintCallable, Category="Garage")
    void SyncVehicleRuntimeState(APlayerVehicle* Vehicle, FName VehicleId);

    UFUNCTION(BlueprintCallable, Category="Garage")
    bool BuildSpawnRecord(FOwnedVehicleRecord& OutRecord) const;

    UPROPERTY(BlueprintAssignable, Category="Garage")
    FOnGarageVehicleSelected OnVehicleSelected;

    UPROPERTY(BlueprintAssignable, Category="Garage")
    FOnGarageStateChanged OnGarageStateChanged;

private:
    void RestoreFromSave();
    void SaveToDisk();

private:
    UPROPERTY()
    TObjectPtr<UVehicleInventory> Inventory;

    UPROPERTY()
    TObjectPtr<USaveGameManager> SaveManager;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Garage", meta=(AllowPrivateAccess="true"))
    FName SelectedVehicleId = TEXT("default");

    bool bIsInGarage = false;
};
