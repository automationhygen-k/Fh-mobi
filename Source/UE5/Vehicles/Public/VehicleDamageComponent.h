#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleDamageComponent.generated.h"

USTRUCT(BlueprintType)
struct FVehicleDamageState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) float EngineDamage = 0.0f;
    UPROPERTY(BlueprintReadOnly) float WheelDamage = 0.0f;
    UPROPERTY(BlueprintReadOnly) float BodyDamage = 0.0f;

    UPROPERTY(BlueprintReadOnly) float EngineCondition = 1.0f;
    UPROPERTY(BlueprintReadOnly) float SteeringStability = 1.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageUpdated, const FVehicleDamageState&, DamageState);

UCLASS(ClassGroup=(Vehicle), meta=(BlueprintSpawnableComponent))
class UVehicleDamageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVehicleDamageComponent();

    UFUNCTION(BlueprintCallable, Category="Vehicle|Damage")
    void ApplyCollisionDamage(float ImpactStrength, bool bHitObstacle, bool bHitTrafficVehicle);

    UFUNCTION(BlueprintCallable, Category="Vehicle|Damage")
    void RepairAll();

    UFUNCTION(BlueprintPure, Category="Vehicle|Damage")
    const FVehicleDamageState& GetState() const { return DamageState; }

    UPROPERTY(BlueprintAssignable, Category="Vehicle|Damage")
    FOnDamageUpdated OnDamageUpdated;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    FVehicleDamageState DamageState;
};
