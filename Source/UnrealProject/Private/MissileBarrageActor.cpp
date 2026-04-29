#include "MissileBarrageActor.h"
#include "BulletBase.h"
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

    if (Targets.Num() > 0 && ProjectileClass)
    {
        FireNextMissile();

        if (Targets.Num() > 1)
        {
            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AMissileBarrageActor::FireNextMissile, FireInterval, true);
        }
    }
    else
    {
        Destroy();
    }
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

        ABulletBase* Missile = GetWorld()->SpawnActor<ABulletBase>(
            ProjectileClass,
            GetActorLocation(),
            GetActorRotation(),
            SpawnParams);

        if (Missile)
        {
            Missile->SetTarget(CurrentTarget);
        }
    }

    CurrentIndex++;

    if (CurrentIndex >= Targets.Num())
    {
        GetWorldTimerManager().ClearTimer(FireTimerHandle);
        Destroy();
    }
}
