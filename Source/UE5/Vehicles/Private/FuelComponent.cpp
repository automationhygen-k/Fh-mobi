#include "FuelComponent.h"

UFuelComponent::UFuelComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UFuelComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentFuel = FMath::Clamp(CurrentFuel, 0.0f, MaxFuel);
}

void UFuelComponent::ConsumeFuel(float DeltaSeconds, float SpeedKmh, float ThrottleInput)
{
    if (CurrentFuel <= 0.0f) return;

    const float SpeedFactor = FMath::Clamp(SpeedKmh / 160.0f, 0.0f, 1.6f);
    const float ThrottleFactor = FMath::Lerp(0.5f, 2.0f, FMath::Clamp(ThrottleInput, 0.0f, 1.0f));
    const float Consumption = BaseConsumptionPerSecond * DeltaSeconds * (0.4f + SpeedFactor) * ThrottleFactor;

    const float PrevNormalized = GetFuelNormalized();
    CurrentFuel = FMath::Max(0.0f, CurrentFuel - Consumption);
    const float NewNormalized = GetFuelNormalized();

    if (!FMath::IsNearlyEqual(PrevNormalized, NewNormalized))
    {
        OnFuelChanged.Broadcast(NewNormalized);
    }

    if (CurrentFuel <= KINDA_SMALL_NUMBER && !bOutOfFuelFired)
    {
        bOutOfFuelFired = true;
        OnOutOfFuel.Broadcast();
    }
}

void UFuelComponent::AddFuel(float Amount)
{
    const float Prev = GetFuelNormalized();
    CurrentFuel = FMath::Clamp(CurrentFuel + FMath::Max(0.0f, Amount), 0.0f, MaxFuel);
    bOutOfFuelFired = CurrentFuel <= KINDA_SMALL_NUMBER;
    const float NewVal = GetFuelNormalized();
    if (!FMath::IsNearlyEqual(Prev, NewVal)) OnFuelChanged.Broadcast(NewVal);
}

void UFuelComponent::RefuelToFull()
{
    const float Prev = GetFuelNormalized();
    CurrentFuel = MaxFuel;
    bOutOfFuelFired = false;
    if (!FMath::IsNearlyEqual(Prev, 1.0f)) OnFuelChanged.Broadcast(1.0f);
}

float UFuelComponent::GetFuelNormalized() const
{
    return MaxFuel > 0.0f ? CurrentFuel / MaxFuel : 0.0f;
}
