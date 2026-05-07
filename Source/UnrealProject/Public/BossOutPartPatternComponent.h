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

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart|Pattern")
	bool IsTemporarilyDisabled() const;

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart|Pattern")
	bool CanStartPattern() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|OutPart|Pattern")
	void DisablePatternForDuration(float DisableDuration);

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart|MiniGame")
	float GetMiniGameSuccessDisableDuration() const { return MiniGameSuccessDisableDuration; }

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon", meta = (ClampMin = "0.0"))
	float SummonWarningDuration = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon", meta = (ClampMin = "0.01"))
	float SummonSpawnInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon", meta = (ClampMin = "0.0"))
	float SummonSpawnForwardDistance = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon", meta = (ClampMin = "0.0"))
	float SummonSpawnLateralSpacing = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon", meta = (ClampMin = "0.0"))
	float SummonSpawnRadius = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon", meta = (ClampMin = "0.0"))
	float SummonSpawnVerticalOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon", meta = (ClampMin = "0.0"))
	float SummonSpawnForwardRandomOffset = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon", meta = (ClampMin = "0.0"))
	float SummonSpawnLateralRandomOffset = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon", meta = (ClampMin = "0.0"))
	float SummonSpawnVerticalRandomOffset = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Summon")
	TArray<TSubclassOf<AActor>> SummonActorClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|MiniGame", meta = (ClampMin = "1"))
	int32 MiniGameTargetCount = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|MiniGame", meta = (ClampMin = "0.0"))
	float MiniGamePatternLockDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|MiniGame", meta = (ClampMin = "0.0"))
	float MiniGameSuccessDisableDuration = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile")
	TSubclassOf<ABulletBase> MissileProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "1"))
	int32 MissileCount = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.01"))
	float MissileFireInterval = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.0"))
	float MissileInitialFireDelay = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.0"))
	float MissileLaunchSpreadAngleDegrees = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.0"))
	float MissileLaunchPitchSpreadAngleDegrees = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.0"))
	float MissileHomingActivationDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.0"))
	float MissileMoveSpeed = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.0"))
	float MissileMaxSpeed = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.0"))
	float MissileSpeedRampDelay = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.0"))
	float MissileSpeedRampInterpSpeed = 1.75f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.0"))
	float MissileTurnInterpSpeed = 3.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile", meta = (ClampMin = "0.0"))
	float MissileHitPoints = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart|Missile")
	bool bMissilesCanBeShotDown = true;

private:
	UPROPERTY()
	TObjectPtr<AMissileBarrageActor> ActiveMissileBarrage;

	TObjectPtr<ABossOutPart> OwnerPart;
	TObjectPtr<UCraftAttackComponent> CommonAttackComponent;
	TObjectPtr<ULaserAttackComponent> LaserAttackComponent;
	FTimerHandle PatternFinishTimerHandle;
	FTimerHandle SummonSequenceTimerHandle;
	bool bPatternActive = false;
	int32 PendingSummonSpawnCount = 0;
	float DisabledUntilTime = 0.0f;

	AActor* ResolveTargetActor(AActor* TargetActor) const;
	void BeginPatternLock(float LockDuration);
	void FinishPattern();
	void SpawnNextSummon();
	bool ExecuteLaserPattern(AActor* TargetActor);
	bool ExecuteSummonPattern();
	bool ExecuteMiniGamePattern();
	bool ExecuteHomingMissilePattern(AActor* TargetActor);
};
