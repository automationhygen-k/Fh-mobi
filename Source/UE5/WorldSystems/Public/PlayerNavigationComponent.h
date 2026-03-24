#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaypointManager.h"
#include "PlayerNavigationComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaypointArrived);

UCLASS(ClassGroup=(Navigation), meta=(BlueprintSpawnableComponent))
class UPlayerNavigationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPlayerNavigationComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="Navigation")
    void SetRouteData(const FNavigationRouteData& InRouteData, const FWaypointData& InWaypointData);

    UFUNCTION(BlueprintPure, Category="Navigation")
    float GetRouteProgress() const { return RouteProgress; }

    UFUNCTION(BlueprintPure, Category="Navigation")
    bool IsRouteActive() const { return bRouteActive; }

    UPROPERTY(BlueprintAssignable, Category="Navigation")
    FOnWaypointArrived OnWaypointArrived;

private:
    void UpdateProgress();

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Navigation", meta=(AllowPrivateAccess="true", ClampMin="50.0"))
    float ArrivalDistance = 140.0f;

    UPROPERTY()
    FNavigationRouteData RouteData;

    UPROPERTY()
    FWaypointData WaypointData;

    float RouteProgress = 0.0f;
    bool bRouteActive = false;
};
