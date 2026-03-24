#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapMarker.generated.h"

UENUM(BlueprintType)
enum class EMapMarkerType : uint8
{
    Settlement,
    FuelStation,
    Garage,
    Mission,
    Player,
    Waypoint
};

UCLASS(Blueprintable)
class AMapMarker : public AActor
{
    GENERATED_BODY()

public:
    AMapMarker();

    UFUNCTION(BlueprintCallable, Category="Map")
    void SetMarkerData(FName InMarkerId, EMapMarkerType InMarkerType, bool bInDiscovered);

    UFUNCTION(BlueprintCallable, Category="Map")
    void SetDiscovered(bool bInDiscovered) { bDiscovered = bInDiscovered; }

    UFUNCTION(BlueprintPure, Category="Map")
    FName GetMarkerId() const { return MarkerId; }

    UFUNCTION(BlueprintPure, Category="Map")
    EMapMarkerType GetMarkerType() const { return MarkerType; }

    UFUNCTION(BlueprintPure, Category="Map")
    bool IsDiscovered() const { return bDiscovered; }

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map", meta=(AllowPrivateAccess="true"))
    FName MarkerId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map", meta=(AllowPrivateAccess="true"))
    EMapMarkerType MarkerType = EMapMarkerType::Settlement;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map", meta=(AllowPrivateAccess="true"))
    bool bDiscovered = false;
};
