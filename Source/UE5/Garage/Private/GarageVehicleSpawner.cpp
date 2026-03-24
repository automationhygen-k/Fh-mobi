#include "GarageVehicleSpawner.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "PlayerVehicle.h"
#include "FuelComponent.h"
#include "VehicleDamageComponent.h"

APlayerVehicle* UGarageVehicleSpawner::SpawnSelectedVehicle(UObject* WorldContextObject, APlayerController* PlayerController, const FOwnedVehicleRecord& VehicleRecord, const FTransform& SpawnTransform)
{
    if (!WorldContextObject || !PlayerController)
    {
        return nullptr;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        return nullptr;
    }

    UClass* SpawnClass = VehicleRecord.VehicleClass.LoadSynchronous();
    if (!SpawnClass)
    {
        SpawnClass = APlayerVehicle::StaticClass();
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APlayerVehicle* Spawned = World->SpawnActor<APlayerVehicle>(SpawnClass, SpawnTransform, Params);
    if (!Spawned)
    {
        return nullptr;
    }

    ApplySavedState(Spawned, VehicleRecord);
    PlayerController->Possess(Spawned);
    PlayerController->SetViewTarget(Spawned);

    return Spawned;
}

void UGarageVehicleSpawner::ApplySavedState(APlayerVehicle* SpawnedVehicle, const FOwnedVehicleRecord& VehicleRecord) const
{
    if (!SpawnedVehicle)
    {
        return;
    }

    if (UFuelComponent* Fuel = SpawnedVehicle->FindComponentByClass<UFuelComponent>())
    {
        Fuel->RefuelToFull();
        if (VehicleRecord.FuelNormalized < 1.0f)
        {
            const float RemoveNorm = FMath::Clamp(1.0f - VehicleRecord.FuelNormalized, 0.0f, 1.0f);
            const float FuelToDrain = Fuel->MaxFuel * RemoveNorm;
            Fuel->ConsumeFuel(FuelToDrain / FMath::Max(Fuel->BaseConsumptionPerSecond, 0.001f), 0.0f, 0.0f);
        }
    }

    if (UVehicleDamageComponent* Damage = SpawnedVehicle->FindComponentByClass<UVehicleDamageComponent>())
    {
        Damage->RepairAll();
        const FVehicleDamageState& Saved = VehicleRecord.DamageState;
        const float Composite = Saved.EngineDamage + Saved.WheelDamage + Saved.BodyDamage;
        if (Composite > 0.001f)
        {
            Damage->ApplyCollisionDamage(Composite * 2000.0f, true, false);
        }
    }
}
