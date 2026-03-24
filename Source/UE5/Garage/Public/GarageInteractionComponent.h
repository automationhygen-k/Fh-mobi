#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GarageInteractionComponent.generated.h"

class UShapeComponent;
class UGarageManager;
class UVehicleRepairService;
class APlayerVehicle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGaragePromptChanged, bool, bShowPrompt);

UCLASS(ClassGroup=(Garage), meta=(BlueprintSpawnableComponent))
class UGarageInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGarageInteractionComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Garage|Interaction")
    void InteractEnterGarage(APlayerVehicle* PlayerVehicle);

    UFUNCTION(BlueprintCallable, Category="Garage|Interaction")
    void InteractExitGarage();

    UFUNCTION(BlueprintCallable, Category="Garage|Interaction")
    void InteractRepairVehicle(APlayerVehicle* PlayerVehicle);

    UFUNCTION(BlueprintCallable, Category="Garage|Interaction")
    void InteractRefuelVehicle(APlayerVehicle* PlayerVehicle);

    UFUNCTION(BlueprintCallable, Category="Garage|Interaction")
    void InteractSelectVehicle(FName VehicleId);

    UPROPERTY(BlueprintAssignable, Category="Garage|Interaction")
    FOnGaragePromptChanged OnGaragePromptChanged;

protected:
    UFUNCTION()
    void HandleTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void HandleTriggerEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Garage|Interaction", meta=(AllowPrivateAccess="true", ClampMin="100.0"))
    float InteractionRadius = 500.0f;

    UPROPERTY()
    TObjectPtr<UShapeComponent> TriggerVolume;

    UPROPERTY()
    TObjectPtr<UGarageManager> GarageManager;

    UPROPERTY()
    TObjectPtr<UVehicleRepairService> RepairService;
};
