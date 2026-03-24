#include "PlayerNavigationComponent.h"

UPlayerNavigationComponent::UPlayerNavigationComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.2f;
}

void UPlayerNavigationComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UPlayerNavigationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bRouteActive)
    {
        UpdateProgress();
    }
}

void UPlayerNavigationComponent::SetRouteData(const FNavigationRouteData& InRouteData, const FWaypointData& InWaypointData)
{
    RouteData = InRouteData;
    WaypointData = InWaypointData;
    RouteProgress = 0.0f;
    bRouteActive = RouteData.bIsValid;
}

void UPlayerNavigationComponent::UpdateProgress()
{
    if (!GetOwner() || !bRouteActive)
    {
        return;
    }

    const FVector Current = GetOwner()->GetActorLocation();
    const float Remaining = FVector::Dist(Current, WaypointData.TargetLocation);

    if (Remaining <= ArrivalDistance)
    {
        bRouteActive = false;
        RouteProgress = 1.0f;
        OnWaypointArrived.Broadcast();
        return;
    }

    const float TotalDistance = FMath::Max(RouteData.DistanceMeters, 1.0f);
    RouteProgress = FMath::Clamp(1.0f - (Remaining / TotalDistance), 0.0f, 0.999f);
}
