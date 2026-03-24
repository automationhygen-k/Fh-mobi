#include "PlayerVehicle.h"

#include "VehicleInputComponent.h"
#include "FuelComponent.h"
#include "VehicleDamageComponent.h"
#include "VehicleCameraManager.h"
#include "VehicleAudioComponent.h"

#include "ChaosWheeledVehicleMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/PlayerController.h"

APlayerVehicle::APlayerVehicle()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.0f;

    VehicleInput = CreateDefaultSubobject<UVehicleInputComponent>(TEXT("VehicleInputComponent"));
    FuelComponent = CreateDefaultSubobject<UFuelComponent>(TEXT("FuelComponent"));
    DamageComponent = CreateDefaultSubobject<UVehicleDamageComponent>(TEXT("VehicleDamageComponent"));
    CameraManager = CreateDefaultSubobject<UVehicleCameraManager>(TEXT("VehicleCameraManager"));
    AudioComponent = CreateDefaultSubobject<UVehicleAudioComponent>(TEXT("VehicleAudioComponent"));
}

void APlayerVehicle::BeginPlay()
{
    Super::BeginPlay();

    if (FuelComponent)
    {
        FuelComponent->OnOutOfFuel.AddDynamic(this, &APlayerVehicle::HandleOutOfFuel);
    }

    if (UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(GetRootComponent()))
    {
        RootPrimitive->OnComponentHit.AddDynamic(this, &APlayerVehicle::OnHitEvent);
    }

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (VehicleInput) VehicleInput->BindPlayerInput(PC);
    }
}

void APlayerVehicle::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UChaosWheeledVehicleMovementComponent* Move = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
    if (!Move || !VehicleInput) return;

    const FVehicleInputState& InputState = VehicleInput->GetInputState();

    if (!bEngineEnabled)
    {
        Move->SetThrottleInput(0.0f);
        Move->SetBrakeInput(1.0f);
    }
    else
    {
        const float AccelScale = DamageComponent ? DamageComponent->GetState().EngineCondition : 1.0f;
        const float SteeringScale = DamageComponent ? DamageComponent->GetState().SteeringStability : 1.0f;

        Move->SetThrottleInput(InputState.Throttle * AccelScale);
        Move->SetBrakeInput(InputState.Brake);
        Move->SetSteeringInput(InputState.Steering * SteeringScale);
        Move->SetHandbrakeInput(InputState.bHandbrake);

        if (InputState.GearOffset > 0) Move->SetTargetGear(Move->GetCurrentGear() + 1, true);
        else if (InputState.GearOffset < 0) Move->SetTargetGear(Move->GetCurrentGear() - 1, true);
    }

    if (VehicleInput) VehicleInput->ClearGearOffset();

    // Environment interaction hook: weather grip multiplier can be consumed by Blueprint/vehicle config.
    (void)WeatherGripMultiplier;

    UpdateStatusData(DeltaSeconds);

    if (FuelComponent && !bRefueling)
    {
        FuelComponent->ConsumeFuel(DeltaSeconds, StatusData.SpeedKmh, FMath::Abs(InputState.Throttle));
    }

    if (AudioComponent)
    {
        const float RPMNorm = FMath::Clamp(StatusData.EngineRPM / 7000.0f, 0.0f, 1.0f);
        AudioComponent->UpdateEngineSound(RPMNorm, FMath::Abs(InputState.Throttle));
    }

    if (CameraManager)
    {
        CameraManager->SetSpeedKmh(StatusData.SpeedKmh);
    }
}

void APlayerVehicle::SetWeatherGripMultiplier(float InGripMultiplier)
{
    WeatherGripMultiplier = FMath::Clamp(InGripMultiplier, 0.45f, 1.2f);
}

void APlayerVehicle::BeginRefuel()
{
    bRefueling = true;
}

void APlayerVehicle::EndRefuel()
{
    bRefueling = false;
}

void APlayerVehicle::HandleOutOfFuel()
{
    bEngineEnabled = false;
}

void APlayerVehicle::OnHitEvent(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    const float Impact = NormalImpulse.Size();
    const bool bObstacle = OtherActor && !OtherActor->ActorHasTag(TEXT("TrafficVehicle"));
    const bool bTraffic = OtherActor && OtherActor->ActorHasTag(TEXT("TrafficVehicle"));

    if (DamageComponent)
    {
        DamageComponent->ApplyCollisionDamage(Impact, bObstacle, bTraffic);
    }

    if (CameraManager)
    {
        CameraManager->OnCollisionShake(FMath::Clamp(Impact / 300000.0f, 0.2f, 2.0f));
    }
}

void APlayerVehicle::UpdateStatusData(float DeltaSeconds)
{
    UChaosWheeledVehicleMovementComponent* Move = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
    if (!Move) return;

    StatusData.SpeedKmh = FMath::Abs(Move->GetForwardSpeed()) * 0.036f;
    StatusData.EngineRPM = Move->GetEngineRotationSpeed();
    StatusData.FuelNormalized = FuelComponent ? FuelComponent->GetFuelNormalized() : 1.0f;
    StatusData.EngineConditionNormalized = DamageComponent ? DamageComponent->GetState().EngineCondition : 1.0f;
    StatusData.bEngineRunning = bEngineEnabled;

    if (!bEngineEnabled && FuelComponent && FuelComponent->HasFuel())
    {
        bEngineEnabled = true;
    }

    // Fuel station interaction hook (for Phase 5 integration via overlap/tag + UI).
    bool bNearFuelStation = false;
    TArray<AActor*> Overlapping;
    GetOverlappingActors(Overlapping);
    for (AActor* A : Overlapping)
    {
        if (A && A->ActorHasTag(TEXT("FuelStation")))
        {
            bNearFuelStation = true;
            break;
        }
    }
    OnRefuelPromptRequested(bNearFuelStation, TEXT("Hold to Refuel"));
}
