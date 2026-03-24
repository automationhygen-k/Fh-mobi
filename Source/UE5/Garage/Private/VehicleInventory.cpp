#include "VehicleInventory.h"

bool UVehicleInventory::AddVehicle(const FOwnedVehicleRecord& VehicleRecord)
{
    if (VehicleRecord.VehicleId.IsNone() || HasVehicle(VehicleRecord.VehicleId))
    {
        return false;
    }

    OwnedVehicles.Add(VehicleRecord);
    OnInventoryChanged.Broadcast();
    return true;
}

bool UVehicleInventory::RemoveVehicleById(FName VehicleId)
{
    const int32 Index = OwnedVehicles.IndexOfByPredicate([VehicleId](const FOwnedVehicleRecord& Item)
    {
        return Item.VehicleId == VehicleId;
    });

    if (Index == INDEX_NONE)
    {
        return false;
    }

    OwnedVehicles.RemoveAt(Index);
    OnInventoryChanged.Broadcast();
    return true;
}

bool UVehicleInventory::UpsertVehicle(const FOwnedVehicleRecord& VehicleRecord)
{
    if (VehicleRecord.VehicleId.IsNone())
    {
        return false;
    }

    FOwnedVehicleRecord* Existing = OwnedVehicles.FindByPredicate([&VehicleRecord](const FOwnedVehicleRecord& Item)
    {
        return Item.VehicleId == VehicleRecord.VehicleId;
    });

    if (Existing)
    {
        *Existing = VehicleRecord;
    }
    else
    {
        OwnedVehicles.Add(VehicleRecord);
    }

    OnInventoryChanged.Broadcast();
    return true;
}

bool UVehicleInventory::HasVehicle(FName VehicleId) const
{
    return OwnedVehicles.ContainsByPredicate([VehicleId](const FOwnedVehicleRecord& Item)
    {
        return Item.VehicleId == VehicleId;
    });
}

bool UVehicleInventory::GetVehicleById(FName VehicleId, FOwnedVehicleRecord& OutVehicleRecord) const
{
    const FOwnedVehicleRecord* Existing = OwnedVehicles.FindByPredicate([VehicleId](const FOwnedVehicleRecord& Item)
    {
        return Item.VehicleId == VehicleId;
    });

    if (!Existing)
    {
        return false;
    }

    OutVehicleRecord = *Existing;
    return true;
}
