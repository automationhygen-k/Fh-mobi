#include "VehicleRepairService.h"

#include "PlayerVehicle.h"
#include "VehicleDamageComponent.h"

bool UVehicleRepairService::RepairVehicle(APlayerVehicle* Vehicle) const
{
    if (!Vehicle)
    {
        return false;
    }

    if (UVehicleDamageComponent* Damage = Vehicle->FindComponentByClass<UVehicleDamageComponent>())
    {
        Damage->RepairAll();
        return true;
    }

    return false;
}
