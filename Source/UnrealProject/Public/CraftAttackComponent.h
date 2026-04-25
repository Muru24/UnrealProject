#pragma once

#include "CoreMinimal.h"
#include "BulletBase.h"
#include "Components/ActorComponent.h"
#include "CraftLoadoutComponent.h"
#include "CraftAttackComponent.generated.h"

class APawn;
class USceneComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UCraftAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCraftAttackComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ApplyLoadoutData(const FCraftLoadoutData& LoadoutData);
	bool FireFromOrigin(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
	bool TryAutoFireFromOrigin(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);

protected:
	TSubclassOf<ABulletBase> ProjectileClass;

	EBulletAttackType AttackType = EBulletAttackType::NonPiercing;

	ECraftAttackPattern AttackPattern = ECraftAttackPattern::Single;

	int32 MaxPenetrationCount = 0;

	float ExplosionRadius = 0.0f;

	int32 BurstCount = 3;

	int32 SpreadCount = 3;

	float SpreadAngle = 18.0f;

	float MultiShotSpacing = 18.0f;

	float AutoFireInterval = 0.35f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float AutoFireCooldownRemaining = 0.0f;

	bool SpawnProjectile(USceneComponent* FireOriginComponent, const FRotator& SpawnRotation, AActor* TargetActor, APawn* InstigatorPawn, float LateralOffset = 0.0f);
	bool FireSingle(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
	bool FireBurst(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
	bool FireSpread(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
};
