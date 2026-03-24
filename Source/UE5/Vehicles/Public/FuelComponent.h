#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FuelComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOutOfFuel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFuelChanged, float, NormalizedFuel);

UCLASS(ClassGroup=(Vehicle), meta=(BlueprintSpawnableComponent))
class UFuelComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFuelComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Vehicle|Fuel")
    void ConsumeFuel(float DeltaSeconds, float SpeedKmh, float ThrottleInput);

    UFUNCTION(BlueprintCallable, Category="Vehicle|Fuel")
    void AddFuel(float Amount);

    UFUNCTION(BlueprintCallable, Category="Vehicle|Fuel")
    void RefuelToFull();

    UFUNCTION(BlueprintPure, Category="Vehicle|Fuel")
    float GetFuelNormalized() const;

    UFUNCTION(BlueprintPure, Category="Vehicle|Fuel")
    bool HasFuel() const { return CurrentFuel > KINDA_SMALL_NUMBER; }

    UPROPERTY(BlueprintAssignable, Category="Vehicle|Fuel")
    FOnOutOfFuel OnOutOfFuel;

    UPROPERTY(BlueprintAssignable, Category="Vehicle|Fuel")
    FOnFuelChanged OnFuelChanged;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle|Fuel", meta=(ClampMin="1.0"))
    float MaxFuel = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle|Fuel", meta=(ClampMin="0.001"))
    float BaseConsumptionPerSecond = 0.008f;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Vehicle|Fuel", meta=(AllowPrivateAccess="true"))
    float CurrentFuel = 60.0f;

    bool bOutOfFuelFired = false;
};
