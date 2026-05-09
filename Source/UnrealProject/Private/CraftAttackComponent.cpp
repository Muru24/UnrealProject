#include "CraftAttackComponent.h"

#include "BulletBase.h"

UCraftAttackComponent::UCraftAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCraftAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FireCooldownRemaining = FMath::Max(0.0f, FireCooldownRemaining - DeltaTime);
	FireRateBuffRemaining = FMath::Max(0.0f, FireRateBuffRemaining - DeltaTime);

	if (FireRateBuffRemaining <= 0.0f)
	{
		CurrentFireRateMultiplier = 1.0f;
	}
}

void UCraftAttackComponent::ApplyAttackConfig(const FCraftAttackConfig& InAttackConfig)
{
	AttackConfig = InAttackConfig;
	AttackConfig.MaxPenetrationCount = FMath::Max(0, AttackConfig.MaxPenetrationCount);
	AttackConfig.ExplosionRadius = FMath::Max(0.0f, AttackConfig.ExplosionRadius);
	AttackConfig.BurstCount = FMath::Max(1, AttackConfig.BurstCount);
	AttackConfig.SpreadCount = FMath::Max(1, AttackConfig.SpreadCount);
	AttackConfig.SpreadAngle = FMath::Max(0.0f, AttackConfig.SpreadAngle);
	AttackConfig.MultiShotSpacing = FMath::Max(0.0f, AttackConfig.MultiShotSpacing);
	AttackConfig.AutoFireInterval = FMath::Max(0.05f, AttackConfig.AutoFireInterval);
}

bool UCraftAttackComponent::FireFromOrigin(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	if (!FireOriginComponent || !AttackConfig.ProjectileClass || !GetWorld() || FireCooldownRemaining > 0.0f)
	{
		return false;
	}

	bool bFiredSuccessfully = false;
	switch (AttackConfig.AttackPattern)
	{
	case ECraftAttackPattern::Burst:
		bFiredSuccessfully = FireBurst(FireOriginComponent, TargetPoint, TargetActor, InstigatorPawn);
		break;
	case ECraftAttackPattern::Spread:
		bFiredSuccessfully = FireSpread(FireOriginComponent, TargetPoint, TargetActor, InstigatorPawn);
		break;
	case ECraftAttackPattern::Single:
	default:
		bFiredSuccessfully = FireSingle(FireOriginComponent, TargetPoint, TargetActor, InstigatorPawn);
		break;
	}

	if (bFiredSuccessfully)
	{
		FireCooldownRemaining = GetCurrentFireInterval();
	}

	return bFiredSuccessfully;
}

bool UCraftAttackComponent::TryAutoFireFromOrigin(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	return FireFromOrigin(FireOriginComponent, TargetPoint, TargetActor, InstigatorPawn);
}

FRotator UCraftAttackComponent::BuildAimRotation(USceneComponent* FireOriginComponent, const FVector& TargetPoint) const
{
	if (!FireOriginComponent)
	{
		return FRotator::ZeroRotator;
	}

	const FVector AimDirection = (TargetPoint - FireOriginComponent->GetComponentLocation()).GetSafeNormal();
	return AimDirection.Rotation();
}

void UCraftAttackComponent::ApplyTemporaryFireRateMultiplier(float InMultiplier, float InDuration)
{
	CurrentFireRateMultiplier = FMath::Max(1.0f, InMultiplier);
	FireRateBuffRemaining = FMath::Max(0.0f, InDuration);
}

float UCraftAttackComponent::GetCurrentFireInterval() const
{
	const float SafeMultiplier = FMath::Max(1.0f, CurrentFireRateMultiplier);
	return AttackConfig.AutoFireInterval / SafeMultiplier;
}

bool UCraftAttackComponent::SpawnProjectile(USceneComponent* FireOriginComponent, const FRotator& SpawnRotation, APawn* InstigatorPawn, float LateralOffset)
{
	if (!FireOriginComponent || !AttackConfig.ProjectileClass || !GetWorld())
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = InstigatorPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector SpawnLocation = FireOriginComponent->GetComponentLocation() + SpawnRotation.RotateVector(FVector(0.0f, LateralOffset, 0.0f));

	ABulletBase* NewBullet = GetWorld()->SpawnActor<ABulletBase>(
		AttackConfig.ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams);

	if (NewBullet)
	{
		NewBullet->ConfigureAttackType(AttackConfig.AttackType, AttackConfig.MaxPenetrationCount, AttackConfig.ExplosionRadius);
	}

	return NewBullet != nullptr;
}

bool UCraftAttackComponent::FireSingle(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	return SpawnProjectile(FireOriginComponent, BuildAimRotation(FireOriginComponent, TargetPoint), InstigatorPawn);
}

bool UCraftAttackComponent::FireBurst(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	const int32 ShotCount = FMath::Max(1, AttackConfig.BurstCount);
	const FRotator BaseRotation = BuildAimRotation(FireOriginComponent, TargetPoint);
	const float StartLateralOffset = -0.5f * static_cast<float>(ShotCount - 1) * AttackConfig.MultiShotSpacing;
	bool bSpawnedAnyProjectile = false;

	for (int32 ShotIndex = 0; ShotIndex < ShotCount; ++ShotIndex)
	{
		const float LateralOffset = StartLateralOffset + static_cast<float>(ShotIndex) * AttackConfig.MultiShotSpacing;
		bSpawnedAnyProjectile |= SpawnProjectile(FireOriginComponent, BaseRotation, InstigatorPawn, LateralOffset);
	}

	return bSpawnedAnyProjectile;
}

bool UCraftAttackComponent::FireSpread(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	const int32 ShotCount = FMath::Max(1, AttackConfig.SpreadCount);
	const FRotator BaseRotation = BuildAimRotation(FireOriginComponent, TargetPoint);

	if (ShotCount == 1)
	{
		return SpawnProjectile(FireOriginComponent, BaseRotation, InstigatorPawn);
	}

	bool bSpawnedAnyProjectile = false;
	const float AngleStep = AttackConfig.SpreadAngle / static_cast<float>(ShotCount - 1);
	const float StartYawOffset = -AttackConfig.SpreadAngle * 0.5f;
	const float StartLateralOffset = -0.5f * static_cast<float>(ShotCount - 1) * AttackConfig.MultiShotSpacing;

	for (int32 ShotIndex = 0; ShotIndex < ShotCount; ++ShotIndex)
	{
		const float YawOffset = StartYawOffset + (AngleStep * ShotIndex);
		const float LateralOffset = StartLateralOffset + static_cast<float>(ShotIndex) * AttackConfig.MultiShotSpacing;
		const FRotator ShotRotation(BaseRotation.Pitch, BaseRotation.Yaw + YawOffset, BaseRotation.Roll);
		bSpawnedAnyProjectile |= SpawnProjectile(FireOriginComponent, ShotRotation, InstigatorPawn, LateralOffset);
	}

	return bSpawnedAnyProjectile;
}
