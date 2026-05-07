#pragma once

#include "CoreMinimal.h"
#include "BulletBase.h"
#include "Components/ActorComponent.h"
#include "EnemyRushComponent.generated.h"

class APawn;
class ABulletBase;
class UPrimitiveComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EEnemyRushBehaviorType : uint8
{
	StraightRush,
	DelayedHoming,
	RangedBurst
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

	float CurrentMoveSpeed = 0.0f;
	float SpeedRampElapsedTime = 0.0f;
	float BehaviorElapsedTime = 0.0f;
	bool bHasTriggeredImpact = false;
	bool bHasFiredBurst = false;
	FVector InitialMoveDirection = FVector::ZeroVector;

	APawn* ResolveTargetPawn() const;
	void HandleImpact(AActor* OtherActor);
	void UpdateMovementTowardTarget(APawn* TargetPawn, float DeltaTime);
	bool TryFireBurst(APawn* TargetPawn);
	void SpawnBurstProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation, APawn* TargetPawn);

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
