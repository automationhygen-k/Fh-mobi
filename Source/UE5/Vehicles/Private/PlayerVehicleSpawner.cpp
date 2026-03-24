#include "PlayerVehicleSpawner.h"

#include "PlayerVehicle.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

APlayerVehicleSpawner::APlayerVehicleSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APlayerVehicleSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (bAutoSpawnOnBeginPlay)
    {
        SpawnPlayerVehicle();
    }
}

APlayerVehicle* APlayerVehicleSpawner::SpawnPlayerVehicle()
{
    UWorld* World = GetWorld();
    if (!World || !PlayerVehicleClass) return nullptr;

    FTransform SpawnTransform = GetActorTransform();
    if (SpawnPointProvider)
    {
        FTransform Provided;
        if (IVehicleSpawnPointProvider::Execute_FindSpawnTransform(SpawnPointProvider.GetObject(), Provided))
        {
            SpawnTransform = Provided;
        }
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    APlayerVehicle* Vehicle = World->SpawnActor<APlayerVehicle>(PlayerVehicleClass, SpawnTransform, Params);
    if (!Vehicle) return nullptr;

    if (APlayerController* PC = World->GetFirstPlayerController())
    {
        PC->Possess(Vehicle);
        PC->SetViewTargetWithBlend(Vehicle, 0.35f);
    }

    return Vehicle;
}
