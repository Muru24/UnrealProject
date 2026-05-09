#pragma once

#include "CoreMinimal.h"
#include "BulletBase.h"
#include "Components/ActorComponent.h"
#include "EnemyRushComponent.generated.h"

class ABulletBase;
class AActor;
class APawn;
class UPrimitiveComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EEnemyRushBehaviorType : uint8
{
	StraightRush,
	DelayedHoming,
	RangedBurst
};

UENUM()
enum class EEnemyRushMoveState : uint8
{
	Chasing,
	Rebounding,
	Returning
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UEnemyRushComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyRushComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush")
	EEnemyRushBehaviorType BehaviorType = EEnemyRushBehaviorType::StraightRush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush", meta = (ClampMin = "0.0"))
	float MoveSpeed = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush", meta = (ClampMin = "0.0"))
	float MaxSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush", meta = (ClampMin = "0.0"))
	float SpeedRampDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush", meta = (ClampMin = "0.0"))
	float SpeedRampInterpSpeed = 1.75f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush", meta = (ClampMin = "0.0"))
	float RotationInterpSpeed = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush", meta = (ClampMin = "0.0"))
	float DelayedHomingDuration = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush", meta = (ClampMin = "0.0"))
	float ReboundDuration = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush", meta = (ClampMin = "0.0"))
	float ReboundSpeedMultiplier = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush", meta = (ClampMin = "0.0"))
	float ReturnSpeedMultiplier = 1.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush", meta = (ClampMin = "0.0"))
	float ReturnAcceptanceRadius = 140.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush|Combat", meta = (ClampMin = "0.0"))
	float ContactDamage = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush|Burst")
	TSubclassOf<ABulletBase> BurstProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush|Burst", meta = (ClampMin = "1"))
	int32 BurstProjectileCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush|Burst", meta = (ClampMin = "0.0"))
	float BurstSpreadAngleDegrees = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush|Burst", meta = (ClampMin = "0.0"))
	float BurstProjectileSpeed = 950.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush|Burst", meta = (ClampMin = "0.0"))
	float BurstTriggerDistance = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Rush|Burst", meta = (ClampMin = "0.0"))
	float BurstProjectileDamage = 8.0f;

private:
	UPROPERTY()
	TObjectPtr<APawn> OwnerPawn;

	UPROPERTY()
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY()
	TObjectPtr<AActor> HomeAnchorActor;

	UPROPERTY()
	TObjectPtr<AActor> CurrentTargetActor;

	float CurrentMoveSpeed = 0.0f;
	float SpeedRampElapsedTime = 0.0f;
	float BehaviorElapsedTime = 0.0f;
	bool bHasFiredBurst = false;
	FVector InitialMoveDirection = FVector::ZeroVector;
	FVector ReboundDirection = FVector::ZeroVector;
	FVector InitialSpawnLocation = FVector::ZeroVector;
	FVector HomeOffsetFromAnchor = FVector::ZeroVector;
	float ReboundElapsedTime = 0.0f;
	EEnemyRushMoveState MoveState = EEnemyRushMoveState::Chasing;

	AActor* ResolveTargetActor() const;
	FVector ResolveHomeLocation() const;
	AActor* AcquireTargetActor();
	void HandleImpact(AActor* OtherActor);
	void UpdateMovementTowardTarget(AActor* TargetActor, float DeltaTime);
	void UpdateReboundMovement(float DeltaTime);
	void UpdateReturnMovement(float DeltaTime);
	void ResetChaseState();
	bool TryFireBurst(AActor* TargetActor);
	void SpawnBurstProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation, AActor* TargetActor);

	UFUNCTION()
	void OnCollisionOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnCollisionHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);
};
