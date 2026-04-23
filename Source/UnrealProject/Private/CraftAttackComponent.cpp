#include "CraftAttackComponent.h"

#include "BulletBase.h"

UCraftAttackComponent::UCraftAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCraftAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AutoFireCooldownRemaining = FMath::Max(0.0f, AutoFireCooldownRemaining - DeltaTime);
}

bool UCraftAttackComponent::FireFromOrigin(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	if (!FireOriginComponent || !ProjectileClass || !GetWorld())
	{
		return false;
	}

	switch (AttackPattern)
	{
	case ECraftAttackPattern::Burst:
		return FireBurst(FireOriginComponent, TargetPoint, TargetActor, InstigatorPawn);
	case ECraftAttackPattern::Spread:
		return FireSpread(FireOriginComponent, TargetPoint, TargetActor, InstigatorPawn);
	case ECraftAttackPattern::Single:
	default:
		return FireSingle(FireOriginComponent, TargetPoint, TargetActor, InstigatorPawn);
	}
}

bool UCraftAttackComponent::TryAutoFireFromOrigin(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	if (AutoFireCooldownRemaining > 0.0f)
	{
		return false;
	}

	if (!FireFromOrigin(FireOriginComponent, TargetPoint, TargetActor, InstigatorPawn))
	{
		return false;
	}

	AutoFireCooldownRemaining = AutoFireInterval;
	return true;
}

bool UCraftAttackComponent::SpawnProjectile(USceneComponent* FireOriginComponent, const FRotator& SpawnRotation, AActor* TargetActor, APawn* InstigatorPawn, float LateralOffset)
{
	if (!FireOriginComponent || !ProjectileClass || !GetWorld())
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = InstigatorPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector SpawnLocation = FireOriginComponent->GetComponentLocation() + SpawnRotation.RotateVector(FVector(0.0f, LateralOffset, 0.0f));

	ABulletBase* NewBullet = GetWorld()->SpawnActor<ABulletBase>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams);

	if (NewBullet && TargetActor)
	{
		NewBullet->SetTarget(TargetActor);
	}

	return NewBullet != nullptr;
}

bool UCraftAttackComponent::FireSingle(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	const FVector AimDirection = (TargetPoint - FireOriginComponent->GetComponentLocation()).GetSafeNormal();
	return SpawnProjectile(FireOriginComponent, AimDirection.Rotation(), TargetActor, InstigatorPawn);
}

bool UCraftAttackComponent::FireBurst(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	const int32 ShotCount = FMath::Max(1, BurstCount);
	const FVector AimDirection = (TargetPoint - FireOriginComponent->GetComponentLocation()).GetSafeNormal();
	const FRotator BaseRotation = AimDirection.Rotation();
	const float StartLateralOffset = -0.5f * static_cast<float>(ShotCount - 1) * MultiShotSpacing;
	bool bSpawnedAnyProjectile = false;

	for (int32 ShotIndex = 0; ShotIndex < ShotCount; ++ShotIndex)
	{
		const float LateralOffset = StartLateralOffset + static_cast<float>(ShotIndex) * MultiShotSpacing;
		bSpawnedAnyProjectile |= SpawnProjectile(FireOriginComponent, BaseRotation, TargetActor, InstigatorPawn, LateralOffset);
	}

	return bSpawnedAnyProjectile;
}

bool UCraftAttackComponent::FireSpread(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	const int32 ShotCount = FMath::Max(1, SpreadCount);
	const FVector AimDirection = (TargetPoint - FireOriginComponent->GetComponentLocation()).GetSafeNormal();
	const FRotator BaseRotation = AimDirection.Rotation();

	if (ShotCount == 1)
	{
		return SpawnProjectile(FireOriginComponent, BaseRotation, TargetActor, InstigatorPawn);
	}

	bool bSpawnedAnyProjectile = false;
	const float AngleStep = SpreadAngle / static_cast<float>(ShotCount - 1);
	const float StartYawOffset = -SpreadAngle * 0.5f;
	const float StartLateralOffset = -0.5f * static_cast<float>(ShotCount - 1) * MultiShotSpacing;

	for (int32 ShotIndex = 0; ShotIndex < ShotCount; ++ShotIndex)
	{
		const float YawOffset = StartYawOffset + (AngleStep * ShotIndex);
		const float LateralOffset = StartLateralOffset + static_cast<float>(ShotIndex) * MultiShotSpacing;
		const FRotator ShotRotation(BaseRotation.Pitch, BaseRotation.Yaw + YawOffset, BaseRotation.Roll);
		bSpawnedAnyProjectile |= SpawnProjectile(FireOriginComponent, ShotRotation, TargetActor, InstigatorPawn, LateralOffset);
	}

	return bSpawnedAnyProjectile;
}
