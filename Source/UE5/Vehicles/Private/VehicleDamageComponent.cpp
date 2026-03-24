#include "VehicleDamageComponent.h"

UVehicleDamageComponent::UVehicleDamageComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UVehicleDamageComponent::ApplyCollisionDamage(float ImpactStrength, bool bHitObstacle, bool bHitTrafficVehicle)
{
    const float Scaled = FMath::Clamp(ImpactStrength / 2000.0f, 0.0f, 1.0f);
    float EngineAdd = Scaled * (bHitObstacle ? 0.35f : 0.2f);
    float WheelAdd = Scaled * (bHitObstacle ? 0.3f : 0.25f);
    float BodyAdd = Scaled * (bHitTrafficVehicle ? 0.35f : 0.2f);

    DamageState.EngineDamage = FMath::Clamp(DamageState.EngineDamage + EngineAdd, 0.0f, 1.0f);
    DamageState.WheelDamage = FMath::Clamp(DamageState.WheelDamage + WheelAdd, 0.0f, 1.0f);
    DamageState.BodyDamage = FMath::Clamp(DamageState.BodyDamage + BodyAdd, 0.0f, 1.0f);

    DamageState.EngineCondition = 1.0f - DamageState.EngineDamage;
    DamageState.SteeringStability = 1.0f - (DamageState.WheelDamage * 0.75f);

    OnDamageUpdated.Broadcast(DamageState);
}

void UVehicleDamageComponent::RepairAll()
{
    DamageState = FVehicleDamageState();
    OnDamageUpdated.Broadcast(DamageState);
}
