#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileBarrageActor.generated.h"

class ABulletBase;
class USceneComponent;

UCLASS()
class UNREALPROJECT_API AMissileBarrageActor : public AActor
{
    GENERATED_BODY()

public:
    AMissileBarrageActor();

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void Init(const TArray<AActor*>& InTargets, TSubclassOf<ABulletBase> InProjectileClass, float InFireInterval);
    void ConfigureMissiles(
        float InMoveSpeed,
        float InMaxSpeed,
        float InTurnInterpSpeed,
        float InHitPoints,
        bool bInCanBeShotDown,
        float InInitialFireDelay,
        float InLaunchSpreadAngleDegrees,
        float InLaunchPitchSpreadAngleDegrees,
        float InHomingActivationDelay,
        float InSpeedRampDelay,
        float InSpeedRampInterpSpeed);

protected:
    void FireNextMissile();

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> Targets;

    UPROPERTY()
    TSubclassOf<ABulletBase> ProjectileClass;

    float FireInterval = 0.1f;
    float MissileMoveSpeed = 650.0f;
    float MissileMaxSpeed = 650.0f;
    float MissileTurnInterpSpeed = 3.5f;
    float MissileHitPoints = 2.0f;
    bool bCanBeShotDown = true;
    float InitialFireDelay = 0.0f;
    float LaunchSpreadAngleDegrees = 0.0f;
    float LaunchPitchSpreadAngleDegrees = 0.0f;
    float HomingActivationDelay = 0.0f;
    float SpawnLateralSpacing = 120.0f;
    float SpawnVerticalSpacing = 45.0f;
    float SpeedRampDelay = 0.0f;
    float SpeedRampInterpSpeed = 0.0f;
    int32 CurrentIndex = 0;
    bool bRepeatFireTimerStarted = false;
    FTimerHandle FireTimerHandle;
};
