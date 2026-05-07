#include "MissileBarrageActor.h"
#include "BulletBase.h"
#include "HomingMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "UnrealProject/P_Player.h"

AMissileBarrageActor::AMissileBarrageActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
}

void AMissileBarrageActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 소유자(PlayerPawn)를 찾아 조준 목록 초기화
    if (AActor* MyOwner = GetOwner())
    {
        if (AP_Player* Player = Cast<AP_Player>(MyOwner->GetOwner()))
        {
            Player->ClearSkillTargetEnemies();
        }
    }

    Super::EndPlay(EndPlayReason);
}

void AMissileBarrageActor::Init(const TArray<AActor*>& InTargets, TSubclassOf<ABulletBase> InProjectileClass, float InFireInterval)
{
    Targets = InTargets;
    ProjectileClass = InProjectileClass;
    FireInterval = InFireInterval;
    CurrentIndex = 0;
    bRepeatFireTimerStarted = false;

    if (Targets.Num() > 0 && ProjectileClass)
    {
        if (InitialFireDelay > 0.0f)
        {
            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AMissileBarrageActor::FireNextMissile, InitialFireDelay, false);
        }
        else
        {
            FireNextMissile();

            if (Targets.Num() > 1)
            {
                GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AMissileBarrageActor::FireNextMissile, FireInterval, true);
                bRepeatFireTimerStarted = true;
            }
        }
    }
    else
    {
        Destroy();
    }
}

void AMissileBarrageActor::ConfigureMissiles(
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
    float InSpeedRampInterpSpeed)
{
    MissileMoveSpeed = FMath::Max(0.0f, InMoveSpeed);
    MissileMaxSpeed = FMath::Max(MissileMoveSpeed, InMaxSpeed);
    MissileTurnInterpSpeed = FMath::Max(0.0f, InTurnInterpSpeed);
    MissileHitPoints = FMath::Max(0.0f, InHitPoints);
    bCanBeShotDown = bInCanBeShotDown;
    InitialFireDelay = FMath::Max(0.0f, InInitialFireDelay);
    LaunchSpreadAngleDegrees = FMath::Max(0.0f, InLaunchSpreadAngleDegrees);
    LaunchPitchSpreadAngleDegrees = FMath::Max(0.0f, InLaunchPitchSpreadAngleDegrees);
    HomingActivationDelay = FMath::Max(0.0f, InHomingActivationDelay);
    SpeedRampDelay = FMath::Max(0.0f, InSpeedRampDelay);
    SpeedRampInterpSpeed = FMath::Max(0.0f, InSpeedRampInterpSpeed);
}

void AMissileBarrageActor::FireNextMissile()
{
    if (CurrentIndex >= Targets.Num() || !ProjectileClass)
    {
        GetWorldTimerManager().ClearTimer(FireTimerHandle);
        Destroy();
        return;
    }

    AActor* CurrentTarget = Targets[CurrentIndex];
    if (IsValid(CurrentTarget))
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = GetOwner();
        SpawnParams.Instigator = Cast<APawn>(GetOwner());
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        const int32 EffectiveCount = FMath::Max(1, Targets.Num());
        const float CenteredIndex = static_cast<float>(CurrentIndex) - ((EffectiveCount - 1) * 0.5f);

        FRotator SpawnRotation = GetActorRotation();
        if (Targets.Num() > 1 && LaunchSpreadAngleDegrees > 0.0f)
        {
            const float SpreadAlpha = static_cast<float>(CurrentIndex) / static_cast<float>(Targets.Num() - 1);
            const float YawOffset = FMath::Lerp(-LaunchSpreadAngleDegrees, LaunchSpreadAngleDegrees, SpreadAlpha);
            SpawnRotation.Yaw += YawOffset;
        }

        if (Targets.Num() > 1 && LaunchPitchSpreadAngleDegrees > 0.0f)
        {
            const float SpreadAlpha = static_cast<float>(CurrentIndex) / static_cast<float>(Targets.Num() - 1);
            const float PitchPattern = (CurrentIndex % 2 == 0) ? -1.0f : 1.0f;
            const float PitchScale = 1.0f - FMath::Abs((SpreadAlpha * 2.0f) - 1.0f);
            SpawnRotation.Pitch += LaunchPitchSpreadAngleDegrees * PitchPattern * FMath::Max(0.35f, PitchScale);
        }

        const FVector SpawnLocation =
            GetActorLocation() +
            (GetActorRightVector() * (CenteredIndex * SpawnLateralSpacing)) +
            (GetActorUpVector() * (((CurrentIndex % 2 == 0) ? -1.0f : 1.0f) * SpawnVerticalSpacing));

        ABulletBase* Missile = GetWorld()->SpawnActor<ABulletBase>(
            ProjectileClass,
            SpawnLocation,
            SpawnRotation,
            SpawnParams);

        if (Missile)
        {
            Missile->SetTarget(CurrentTarget);
            Missile->SetProjectileSpeed(MissileMoveSpeed, MissileMaxSpeed);
            Missile->ConfigureSpeedRamp(SpeedRampDelay, SpeedRampInterpSpeed);
            Missile->ConfigureShotDown(MissileHitPoints, bCanBeShotDown);

            if (UHomingMovementComponent* HomingMovementComponent = Missile->FindComponentByClass<UHomingMovementComponent>())
            {
                HomingMovementComponent->SetTurnInterpSpeed(MissileTurnInterpSpeed);
                HomingMovementComponent->SetHomingActivationDelay(HomingActivationDelay);
            }
        }
    }

    CurrentIndex++;

    if (CurrentIndex >= Targets.Num())
    {
        GetWorldTimerManager().ClearTimer(FireTimerHandle);
        Destroy();
        return;
    }

    if (!bRepeatFireTimerStarted && Targets.Num() > 1)
    {
        GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AMissileBarrageActor::FireNextMissile, FireInterval, true);
        bRepeatFireTimerStarted = true;
    }
}
