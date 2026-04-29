#include "SkillEffect_MultiHomingMissile.h"
#include "BulletBase.h"
#include "EnemyManager.h"
#include "MissileBarrageActor.h"
#include "Engine/World.h"
#include "SquadCraftActor.h"
#include "UnrealProject/P_Player.h"

bool USkillEffect_MultiHomingMissile::ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration)
{
    if (!SourceActor || !ProjectileClass) return false;

    UWorld* World = SourceActor->GetWorld();
    if (!World) return false;

    // 플레이어 포인터 획득 (HUD 연동용)
    AP_Player* Player = nullptr;
    if (SourceActor->GetOwner())
    {
        Player = Cast<AP_Player>(SourceActor->GetOwner());
    }

    // 1. EnemyManager에서 타겟 획득
    UEnemyManager* EnemyManager = World->GetSubsystem<UEnemyManager>();
    if (!EnemyManager) return false;

    const TArray<APawn*>& Enemies = EnemyManager->GetEnemys();
    if (Enemies.Num() == 0) return false;

    // 2. 최대 10개의 타겟 랜덤 선정 (중복 허용)
    TArray<AActor*> SelectedTargets;
    for (int32 i = 0; i < MaxMissiles; ++i)
    {
        int32 RandomIndex = FMath::RandRange(0, Enemies.Num() - 1);
        if (Enemies[RandomIndex])
        {
            SelectedTargets.Add(Enemies[RandomIndex]);
        }
    }

    if (SelectedTargets.Num() == 0) return false;

    // 플레이어의 조준 목록에 등록 (HUD 표시용)
    if (Player)
    {
        Player->SetSkillTargetEnemies(SelectedTargets);
    }

    // 3. 기체의 FireOrigin 위치 획득
    FVector SpawnLocation = SourceActor->GetActorLocation();
    FRotator SpawnRotation = SourceActor->GetActorRotation();

    if (ASquadCraftActor* Craft = Cast<ASquadCraftActor>(SourceActor))
    {
        if (USceneComponent* FireOrigin = Craft->GetFireOrigin())
        {
            SpawnLocation = FireOrigin->GetComponentLocation();
            SpawnRotation = FireOrigin->GetComponentRotation();
        }
    }

    // 4. 발사 매니저 스폰
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = SourceActor;
    SpawnParams.Instigator = Cast<APawn>(SourceActor);
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AMissileBarrageActor* BarrageManager = World->SpawnActor<AMissileBarrageActor>(
        AMissileBarrageActor::StaticClass(),
        SpawnLocation,
        SpawnRotation,
        SpawnParams);

    if (BarrageManager)
    {
        // 매니저를 기체에 부착하여 이동 중에도 발사 위치가 유지되도록 함
        FAttachmentTransformRules AttachRules(EAttachmentRule::KeepWorld, false);
        BarrageManager->AttachToActor(SourceActor, AttachRules);

        // 만약 FireOrigin이 있다면 해당 컴포넌트에 더 정확히 부착 가능
        if (ASquadCraftActor* Craft = Cast<ASquadCraftActor>(SourceActor))
        {
            if (USceneComponent* FireOrigin = Craft->GetFireOrigin())
            {
                BarrageManager->AttachToComponent(FireOrigin, FAttachmentTransformRules::KeepWorldTransform);
            }
        }

        BarrageManager->Init(SelectedTargets, ProjectileClass, FireInterval);
        return true;
    }

    return false;
}
