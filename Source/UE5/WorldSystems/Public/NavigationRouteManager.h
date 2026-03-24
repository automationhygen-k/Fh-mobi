#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NavigationRouteManager.generated.h"

USTRUCT(BlueprintType)
struct FNavigationRouteData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Navigation")
    TArray<FVector> PathNodes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Navigation")
    float DistanceMeters = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Navigation")
    float EstimatedTravelTimeSeconds = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Navigation")
    bool bIsValid = false;
};

UINTERFACE(BlueprintType)
class URoadRouteProvider : public UInterface
{
    GENERATED_BODY()
};

class IRoadRouteProvider
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Navigation")
    bool BuildRoadRoute(const FVector& Start, const FVector& End, TArray<FVector>& OutNodes) const;
};

UCLASS(BlueprintType)
class UNavigationRouteManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Navigation")
    FNavigationRouteData GetOrBuildRoute(const FVector& Start, const FVector& End, const TScriptInterface<IRoadRouteProvider>& RouteProvider);

    UFUNCTION(BlueprintCallable, Category="Navigation")
    void InvalidateCache();

private:
    static FString BuildKey(const FVector& Start, const FVector& End);
    static FNavigationRouteData BuildFallbackRoute(const FVector& Start, const FVector& End);

private:
    UPROPERTY()
    TMap<FString, FNavigationRouteData> RouteCache;
};
