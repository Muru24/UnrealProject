#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftCombatTypes.h"
#include "BossOutPartPatternComponent.generated.h"

class AActor;
class ABossOutPart;
class ABulletBase;
class AMissileBarrageActor;
class ULaserAttackComponent;
class UCraftAttackComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBossOutPartPatternFinished, UBossOutPartPatternComponent*);

UENUM(BlueprintType)
enum class EBossOutPartPatternType : uint8
{
	Laser,
	SummonAdds,
	MiniGame,
	HomingMissile
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UBossOutPartPatternComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossOutPartPatternComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Boss|OutPart|Pattern")
	bool FireCommonPattern(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Boss|OutPart|Pattern")
	bool ExecuteSpecialPattern(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Boss|OutPart|Pattern")
	void StopActivePattern();

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart|Pattern")
	EBossOutPartPatternType GetPatternType() const { return PatternType; }

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart|Pattern")
	bool IsPatternActive() const { return bPatternActive; }

	FOnBossOutPartPatternFinished OnPatternFinished;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Pattern")
	EBossOutPartPatternType PatternType = EBossOutPartPatternType::Laser;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Pattern")
	FCraftAttackConfig CommonAttackConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Pattern", meta = (ClampMin = "0.0"))
	float CommonPatternLockDuration = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Laser")
	float LaserDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Laser", meta = (ClampMin = "0.0"))
	float LaserRecoveryDuration = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon", meta = (ClampMin = "1"))
	int32 SummonCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon", meta = (ClampMin = "0.0"))
	float SummonPatternLockDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon")
	TArray<TSubclassOf<AActor>> SummonActorClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|MiniGame", meta = (ClampMin = "1"))
	int32 MiniGameTargetCount = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|MiniGame", meta = (ClampMin = "0.0"))
	float MiniGamePatternLockDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile")
	TSubclassOf<ABulletBase> MissileProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "1"))
	int32 MissileCount = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.01"))
	float MissileFireInterval = 0.15f;

private:
	UPROPERTY()
	TObjectPtr<AMissileBarrageActor> ActiveMissileBarrage;

	TObjectPtr<ABossOutPart> OwnerPart;
	TObjectPtr<UCraftAttackComponent> CommonAttackComponent;
	TObjectPtr<ULaserAttackComponent> LaserAttackComponent;
	FTimerHandle PatternFinishTimerHandle;
	bool bPatternActive = false;

	AActor* ResolveTargetActor(AActor* TargetActor) const;
	void BeginPatternLock(float LockDuration);
	void FinishPattern();
	bool ExecuteLaserPattern(AActor* TargetActor);
	bool ExecuteSummonPattern();
	bool ExecuteMiniGamePattern();
	bool ExecuteHomingMissilePattern(AActor* TargetActor);
};
