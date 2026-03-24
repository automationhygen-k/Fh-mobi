#include "GarageInteractionComponent.h"

#include "GarageManager.h"
#include "VehicleRepairService.h"
#include "PlayerVehicle.h"
#include "FuelComponent.h"

#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"

UGarageInteractionComponent::UGarageInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UGarageInteractionComponent::BeginPlay()
{
    Super::BeginPlay();

    GarageManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<UGarageManager>() : nullptr;
    RepairService = NewObject<UVehicleRepairService>(this, TEXT("VehicleRepairService"));

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    USphereComponent* Sphere = NewObject<USphereComponent>(Owner, TEXT("GarageInteractionTrigger"));
    Sphere->SetSphereRadius(InteractionRadius);
    Sphere->SetCollisionProfileName(TEXT("Trigger"));
    Sphere->SetupAttachment(Owner->GetRootComponent());
    Sphere->RegisterComponent();
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &UGarageInteractionComponent::HandleTriggerBegin);
    Sphere->OnComponentEndOverlap.AddDynamic(this, &UGarageInteractionComponent::HandleTriggerEnd);
    TriggerVolume = Sphere;
}

void UGarageInteractionComponent::InteractEnterGarage(APlayerVehicle* PlayerVehicle)
{
    if (!GarageManager)
    {
        return;
    }

    GarageManager->EnterGarage();
    GarageManager->SyncVehicleRuntimeState(PlayerVehicle, GarageManager->GetSelectedVehicleId());
}

void UGarageInteractionComponent::InteractExitGarage()
{
    if (GarageManager)
    {
        GarageManager->ExitGarage();
    }
}

void UGarageInteractionComponent::InteractRepairVehicle(APlayerVehicle* PlayerVehicle)
{
    if (RepairService)
    {
        RepairService->RepairVehicle(PlayerVehicle);
    }
}

void UGarageInteractionComponent::InteractRefuelVehicle(APlayerVehicle* PlayerVehicle)
{
    if (!PlayerVehicle)
    {
        return;
    }

    if (UFuelComponent* Fuel = PlayerVehicle->FindComponentByClass<UFuelComponent>())
    {
        Fuel->RefuelToFull();
    }

    if (GarageManager)
    {
        GarageManager->SyncVehicleRuntimeState(PlayerVehicle, GarageManager->GetSelectedVehicleId());
    }
}

void UGarageInteractionComponent::InteractSelectVehicle(FName VehicleId)
{
    if (GarageManager)
    {
        GarageManager->SelectVehicle(VehicleId);
    }
}

void UGarageInteractionComponent::HandleTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA<APlayerVehicle>())
    {
        OnGaragePromptChanged.Broadcast(true);
    }
}

void UGarageInteractionComponent::HandleTriggerEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->IsA<APlayerVehicle>())
    {
        OnGaragePromptChanged.Broadcast(false);
    }
}
