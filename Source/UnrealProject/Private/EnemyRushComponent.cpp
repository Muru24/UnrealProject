#include "EnemyRushComponent.h"

#include "BulletBase.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "StatComponent.h"

UEnemyRushComponent::UEnemyRushComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyRushComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	CollisionComponent = OwnerPawn->FindComponentByClass<USphereComponent>();
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &UEnemyRushComponent::OnCollisionOverlap);
		CollisionComponent->OnComponentHit.AddDynamic(this, &UEnemyRushComponent::OnCollisionHit);
	}

	CurrentMoveSpeed = FMath::Max(0.0f, MoveSpeed);
	SpeedRampElapsedTime = 0.0f;
	BehaviorElapsedTime = 0.0f;
	bHasTriggeredImpact = false;
	bHasFiredBurst = false;

	if (APawn* TargetPawn = ResolveTargetPawn())
	{
		InitialMoveDirection = (TargetPawn->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal();
	}

	if (InitialMoveDirection.IsNearlyZero())
	{
		InitialMoveDirection = OwnerPawn->GetActorForwardVector();
	}
}

void UEnemyRushComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(OwnerPawn) || bHasTriggeredImpact)
	{
		return;
	}

	APawn* TargetPawn = ResolveTargetPawn();
	if (!IsValid(TargetPawn))
	{
		return;
	}

	SpeedRampElapsedTime += DeltaTime;
	BehaviorElapsedTime += DeltaTime;
	if (CurrentMoveSpeed < MaxSpeed && SpeedRampInterpSpeed > 0.0f && SpeedRampElapsedTime >= SpeedRampDelay)
	{
		CurrentMoveSpeed = FMath::FInterpTo(CurrentMoveSpeed, MaxSpeed, DeltaTime, SpeedRampInterpSpeed);
	}

	if (BehaviorType == EEnemyRushBehaviorType::RangedBurst)
	{
		const float DistanceToTarget = FVector::Dist(OwnerPawn->GetActorLocation(), TargetPawn->GetActorLocation());
		if (!bHasFiredBurst && DistanceToTarget <= BurstTriggerDistance)
		{
			if (TryFireBurst(TargetPawn))
			{
				bHasFiredBurst = true;
				OwnerPawn->Destroy();
			}
			return;
		}
	}

	UpdateMovementTowardTarget(TargetPawn, DeltaTime);
}

APawn* UEnemyRushComponent::ResolveTargetPawn() const
{
	return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void UEnemyRushComponent::HandleImpact(AActor* OtherActor)
{
	if (bHasTriggeredImpact || !IsValid(OwnerPawn) || !IsValid(OtherActor) || OtherActor == OwnerPawn)
	{
		return;
	}

	APawn* TargetPawn = ResolveTargetPawn();
	if (OtherActor != TargetPawn)
	{
		return;
	}

	if (UStatComponent* TargetStatComponent = OtherActor->FindComponentByClass<UStatComponent>())
	{
		TargetStatComponent->ApplyDamage(ContactDamage);
	}

	bHasTriggeredImpact = true;
	OwnerPawn->Destroy();
}

void UEnemyRushComponent::UpdateMovementTowardTarget(APawn* TargetPawn, float DeltaTime)
{
	FVector DesiredDirection = FVector::ZeroVector;
	if (BehaviorType == EEnemyRushBehaviorType::DelayedHoming && BehaviorElapsedTime < DelayedHomingDuration)
	{
		DesiredDirection = InitialMoveDirection;
	}
	else
	{
		DesiredDirection = (TargetPawn->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal();
	}

	if (!DesiredDirection.IsNearlyZero())
	{
		const FRotator TargetRotation = DesiredDirection.Rotation();
		const FRotator NewRotation = FMath::RInterpTo(OwnerPawn->GetActorRotation(), TargetRotation, DeltaTime, RotationInterpSpeed);
		OwnerPawn->SetActorRotation(NewRotation);
	}

	const FVector MoveStep = OwnerPawn->GetActorForwardVector() * CurrentMoveSpeed * DeltaTime;
	FHitResult SweepHit;
	OwnerPawn->AddActorWorldOffset(MoveStep, true, &SweepHit);

	if (SweepHit.GetActor())
	{
		HandleImpact(SweepHit.GetActor());
	}
}

bool UEnemyRushComponent::TryFireBurst(APawn* TargetPawn)
{
	if (!IsValid(OwnerPawn) || !IsValid(TargetPawn) || !BurstProjectileClass || BurstProjectileCount <= 0)
	{
		return false;
	}

	const FVector ToTarget = (TargetPawn->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal();
	if (ToTarget.IsNearlyZero())
	{
		return false;
	}

	const FRotator BaseRotation = ToTarget.Rotation();
	const FVector SpawnLocation = OwnerPawn->GetActorLocation() + (OwnerPawn->GetActorForwardVector() * 110.0f);

	for (int32 ProjectileIndex = 0; ProjectileIndex < BurstProjectileCount; ++ProjectileIndex)
	{
		const float SpreadAlpha = (BurstProjectileCount == 1)
			? 0.5f
			: static_cast<float>(ProjectileIndex) / static_cast<float>(BurstProjectileCount - 1);
		const float YawOffset = FMath::Lerp(-BurstSpreadAngleDegrees, BurstSpreadAngleDegrees, SpreadAlpha);

		FRotator SpawnRotation = BaseRotation;
		SpawnRotation.Yaw += YawOffset;
		SpawnBurstProjectile(SpawnLocation, SpawnRotation, TargetPawn);
	}

	return true;
}

void UEnemyRushComponent::SpawnBurstProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation, APawn* TargetPawn)
{
	if (!GetWorld())
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPawn;
	SpawnParams.Instigator = OwnerPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABulletBase* SpawnedProjectile = GetWorld()->SpawnActor<ABulletBase>(
		BurstProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams);

	if (!SpawnedProjectile)
	{
		return;
	}

	SpawnedProjectile->SetTarget(TargetPawn);
	SpawnedProjectile->SetProjectileSpeed(BurstProjectileSpeed, BurstProjectileSpeed);
	SpawnedProjectile->ConfigureAttackType(EBulletAttackType::NonPiercing, 0, 0.0f);
}

void UEnemyRushComponent::OnCollisionOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	HandleImpact(OtherActor);
}

void UEnemyRushComponent::OnCollisionHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	HandleImpact(OtherActor);
}
