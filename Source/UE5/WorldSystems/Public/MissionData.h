#pragma once

#include "CoreMinimal.h"
#include "MissionData.generated.h"

UENUM(BlueprintType)
enum class EMissionType : uint8
{
    Delivery UMETA(DisplayName="Delivery"),
    Exploration UMETA(DisplayName="Exploration"),
    Assistance UMETA(DisplayName="Assistance")
};

USTRUCT(BlueprintType)
struct FMissionData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    FName MissionId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    EMissionType MissionType = EMissionType::Delivery;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    FVector ObjectiveLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    float RewardValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    float CompletionRadius = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    bool bCompleted = false;
};
