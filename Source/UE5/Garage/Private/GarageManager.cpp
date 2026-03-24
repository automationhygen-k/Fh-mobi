#include "GarageManager.h"

#include "SaveGameManager.h"
#include "PlayerVehicle.h"
#include "FuelComponent.h"
#include "VehicleDamageComponent.h"

void UGarageManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    Inventory = NewObject<UVehicleInventory>(this, TEXT("VehicleInventory"));
    SaveManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveGameManager>() : nullptr;

    RestoreFromSave();
}

void UGarageManager::RegisterIntroDeliveredVehicle(TSoftClassPtr<APawn> VehicleClass, FName VehicleId)
{
    FOwnedVehicleRecord Record;
    Record.VehicleId = VehicleId;
    Record.VehicleClass = VehicleClass;
    Record.LastUsedTime = FDateTime::UtcNow();

    Inventory->UpsertVehicle(Record);
    SelectedVehicleId = VehicleId;

    SaveToDisk();
    OnVehicleSelected.Broadcast(SelectedVehicleId);
}

bool UGarageManager::SelectVehicle(FName VehicleId)
{
    if (!Inventory || !Inventory->HasVehicle(VehicleId))
    {
        return false;
    }

    SelectedVehicleId = VehicleId;
    SaveToDisk();
    OnVehicleSelected.Broadcast(SelectedVehicleId);
    return true;
}

void UGarageManager::EnterGarage()
{
    if (bIsInGarage)
    {
        return;
    }

    bIsInGarage = true;
    SaveToDisk();
    OnGarageStateChanged.Broadcast(true);
}

void UGarageManager::ExitGarage()
{
    if (!bIsInGarage)
    {
        return;
    }

    bIsInGarage = false;
    SaveToDisk();
    OnGarageStateChanged.Broadcast(false);
}

void UGarageManager::SyncVehicleRuntimeState(APlayerVehicle* Vehicle, FName VehicleId)
{
    if (!Vehicle || !Inventory)
    {
        return;
    }

    FOwnedVehicleRecord Record;
    if (!Inventory->GetVehicleById(VehicleId, Record))
    {
        Record.VehicleId = VehicleId;
        Record.VehicleClass = Vehicle->GetClass();
    }

    if (const UFuelComponent* Fuel = Vehicle->FindComponentByClass<UFuelComponent>())
    {
        Record.FuelNormalized = Fuel->GetFuelNormalized();
    }

    if (const UVehicleDamageComponent* Damage = Vehicle->FindComponentByClass<UVehicleDamageComponent>())
    {
        Record.DamageState = Damage->GetState();
    }

    Record.LastUsedTime = FDateTime::UtcNow();
    Inventory->UpsertVehicle(Record);

    if (VehicleId == SelectedVehicleId)
    {
        SaveToDisk();
    }
}

bool UGarageManager::BuildSpawnRecord(FOwnedVehicleRecord& OutRecord) const
{
    return Inventory && Inventory->GetVehicleById(SelectedVehicleId, OutRecord);
}

void UGarageManager::RestoreFromSave()
{
    if (!SaveManager || !Inventory)
    {
        return;
    }

    const FGarageSaveData& Data = SaveManager->GetSaveData();
    SelectedVehicleId = Data.SelectedVehicleId;

    for (const FOwnedVehicleRecord& Item : Data.Vehicles)
    {
        Inventory->UpsertVehicle(Item);
    }
}

void UGarageManager::SaveToDisk()
{
    if (!SaveManager || !Inventory)
    {
        return;
    }

    FGarageSaveData Data = SaveManager->GetSaveData();
    Data.SelectedVehicleId = SelectedVehicleId;
    Data.Vehicles = Inventory->GetAllVehicles();
    Data.bFirstLaunch = false;

    SaveManager->SetSaveData(Data);
    SaveManager->SaveNow();
}
