#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileBarrageActor.generated.h"

class ABulletBase;

UCLASS()
class UNREALPROJECT_API AMissileBarrageActor : public AActor
{
    GENERATED_BODY()

public:
    AMissileBarrageActor();

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // 발사 매니저 초기화: 타겟 목록, 총알 클래스, 발사 간격 설정
    void Init(const TArray<AActor*>& InTargets, TSubclassOf<ABulletBase> InProjectileClass, float InFireInterval);

protected:
    void FireNextMissile();

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class USceneComponent* SceneRoot;

    UPROPERTY()
    TArray<AActor*> Targets;

    UPROPERTY()
    TSubclassOf<ABulletBase> ProjectileClass;

    float FireInterval = 0.1f;
    int32 CurrentIndex = 0;
    FTimerHandle FireTimerHandle;
};
