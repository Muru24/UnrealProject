#include "EnemyRushComponent.h"

#include "UnrealProject/P_Player.h"
#include "UnrealProject/Pawn_Template.h"
#include "BulletBase.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "SquadCraftActor.h"
#include "SquadRuntimeComponent.h"
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
		CollisionComponent->SetGenerateOverlapEvents(true);
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &UEnemyRushComponent::OnCollisionOverlap);
		CollisionComponent->OnComponentHit.AddDynamic(this, &UEnemyRushComponent::OnCollisionHit);
	}

	HomeAnchorActor = OwnerPawn->GetOwner();
	InitialSpawnLocation = OwnerPawn->GetActorLocation();
	HomeOffsetFromAnchor = IsValid(HomeAnchorActor)
		? (InitialSpawnLocation - HomeAnchorActor->GetActorLocation())
		: FVector::ZeroVector;

	CurrentMoveSpeed = FMath::Max(0.0f, MoveSpeed);
	SpeedRampElapsedTime = 0.0f;
	BehaviorElapsedTime = 0.0f;
	bHasFiredBurst = false;
	ReboundElapsedTime = 0.0f;
	MoveState = EEnemyRushMoveState::Chasing;
	CurrentTargetActor = nullptr;

	if (AActor* TargetActor = AcquireTargetActor())
	{
		InitialMoveDirection = (TargetActor->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal();
	}

	if (InitialMoveDirection.IsNearlyZero())
	{
		InitialMoveDirection = OwnerPawn->GetActorForwardVector();
	}
}

void UEnemyRushComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(OwnerPawn))
	{
		return;
	}

	if (const APawn_Template* OwnerPawnTemplate = Cast<APawn_Template>(OwnerPawn))
	{
		if (!OwnerPawnTemplate->IsDissolveInComplete())
		{
			return;
		}
	}

	if (MoveState == EEnemyRushMoveState::Rebounding)
	{
		UpdateReboundMovement(DeltaTime);
		return;
	}

	if (MoveState == EEnemyRushMoveState::Returning)
	{
		UpdateReturnMovement(DeltaTime);
		return;
	}

	AActor* TargetActor = AcquireTargetActor();
	if (!IsValid(TargetActor))
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
		const float DistanceToTarget = FVector::Dist(OwnerPawn->GetActorLocation(), TargetActor->GetActorLocation());
		if (!bHasFiredBurst && DistanceToTarget <= BurstTriggerDistance)
		{
			if (TryFireBurst(TargetActor))
			{
				bHasFiredBurst = true;
				OwnerPawn->Destroy();
			}
			return;
		}
	}

	UpdateMovementTowardTarget(TargetActor, DeltaTime);
}

AActor* UEnemyRushComponent::ResolveTargetActor() const
{
	AP_Player* PlayerPawn = Cast<AP_Player>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerPawn)
	{
		return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}

	if (USquadRuntimeComponent* SquadRuntime = PlayerPawn->FindComponentByClass<USquadRuntimeComponent>())
	{
		TArray<ASquadCraftActor*> AllCrafts;
		SquadRuntime->GetAllCrafts(AllCrafts);

		TArray<ASquadCraftActor*> ValidCrafts;
		for (ASquadCraftActor* Craft : AllCrafts)
		{
			if (IsValid(Craft) && Craft->IsOperational())
			{
				ValidCrafts.Add(Craft);
			}
		}

		if (!ValidCrafts.IsEmpty())
		{
			return ValidCrafts[FMath::RandRange(0, ValidCrafts.Num() - 1)];
		}
	}

	return PlayerPawn;
}

AActor* UEnemyRushComponent::AcquireTargetActor()
{
	if (IsValid(CurrentTargetActor))
	{
		if (const ASquadCraftActor* Craft = Cast<ASquadCraftActor>(CurrentTargetActor.Get()))
		{
			if (Craft->IsOperational())
			{
				return CurrentTargetActor.Get();
			}
		}
		else
		{
			return CurrentTargetActor.Get();
		}
	}

	CurrentTargetActor = ResolveTargetActor();
	return CurrentTargetActor.Get();
}

void UEnemyRushComponent::HandleImpact(AActor* OtherActor)
{
	if (MoveState != EEnemyRushMoveState::Chasing || !IsValid(OwnerPawn) || !IsValid(OtherActor) || OtherActor == OwnerPawn)
	{
		return;
	}

	UStatComponent* TargetStatComponent = OtherActor->FindComponentByClass<UStatComponent>();
	if (!TargetStatComponent)
	{
		return;
	}

	TargetStatComponent->ApplyDamage(ContactDamage);

	ReboundDirection = OwnerPawn->GetActorForwardVector();
	if (ReboundDirection.IsNearlyZero())
	{
		ReboundDirection = (OwnerPawn->GetActorLocation() - OtherActor->GetActorLocation()).GetSafeNormal();
	}
	if (ReboundDirection.IsNearlyZero())
	{
		ReboundDirection = FVector::ForwardVector;
	}

	MoveState = EEnemyRushMoveState::Rebounding;
	ReboundElapsedTime = 0.0f;
}

void UEnemyRushComponent::UpdateMovementTowardTarget(AActor* TargetActor, float DeltaTime)
{
	FVector DesiredDirection = FVector::ZeroVector;
	if (BehaviorType == EEnemyRushBehaviorType::DelayedHoming && BehaviorElapsedTime < DelayedHomingDuration)
	{
		DesiredDirection = InitialMoveDirection;
	}
	else
	{
		DesiredDirection = (TargetActor->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal();
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

FVector UEnemyRushComponent::ResolveHomeLocation() const
{
	if (IsValid(HomeAnchorActor))
	{
		return HomeAnchorActor->GetActorLocation() + HomeOffsetFromAnchor;
	}

	return InitialSpawnLocation;
}

void UEnemyRushComponent::UpdateReboundMovement(float DeltaTime)
{
	ReboundElapsedTime += DeltaTime;

	const FVector SafeDirection = ReboundDirection.GetSafeNormal();
	if (!SafeDirection.IsNearlyZero())
	{
		const FRotator TargetRotation = SafeDirection.Rotation();
		const FRotator NewRotation = FMath::RInterpTo(OwnerPawn->GetActorRotation(), TargetRotation, DeltaTime, RotationInterpSpeed);
		OwnerPawn->SetActorRotation(NewRotation);
	}

	const float ReboundSpeed = FMath::Max(CurrentMoveSpeed, MoveSpeed) * FMath::Max(1.0f, ReboundSpeedMultiplier);
	const FVector MoveStep = OwnerPawn->GetActorForwardVector() * ReboundSpeed * DeltaTime;
	OwnerPawn->AddActorWorldOffset(MoveStep, true);

	if (ReboundElapsedTime >= ReboundDuration)
	{
		MoveState = EEnemyRushMoveState::Returning;
	}
}

void UEnemyRushComponent::UpdateReturnMovement(float DeltaTime)
{
	const FVector HomeLocation = ResolveHomeLocation();
	const FVector ToHome = HomeLocation - OwnerPawn->GetActorLocation();
	const float DistanceToHome = ToHome.Size();

	if (DistanceToHome <= ReturnAcceptanceRadius)
	{
		ResetChaseState();
		return;
	}

	const FVector DesiredDirection = ToHome.GetSafeNormal();
	if (!DesiredDirection.IsNearlyZero())
	{
		const FRotator TargetRotation = DesiredDirection.Rotation();
		const FRotator NewRotation = FMath::RInterpTo(OwnerPawn->GetActorRotation(), TargetRotation, DeltaTime, RotationInterpSpeed);
		OwnerPawn->SetActorRotation(NewRotation);
	}

	const float ReturnSpeed = FMath::Max(MoveSpeed, CurrentMoveSpeed) * FMath::Max(1.0f, ReturnSpeedMultiplier);
	const FVector MoveStep = OwnerPawn->GetActorForwardVector() * ReturnSpeed * DeltaTime;
	OwnerPawn->AddActorWorldOffset(MoveStep, true);
}

void UEnemyRushComponent::ResetChaseState()
{
	MoveState = EEnemyRushMoveState::Chasing;
	CurrentMoveSpeed = FMath::Max(0.0f, MoveSpeed);
	SpeedRampElapsedTime = 0.0f;
	BehaviorElapsedTime = 0.0f;
	ReboundElapsedTime = 0.0f;
	CurrentTargetActor = nullptr;

	if (AActor* TargetActor = AcquireTargetActor())
	{
		InitialMoveDirection = (TargetActor->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal();
	}
	else
	{
		InitialMoveDirection = OwnerPawn->GetActorForwardVector();
	}
}

bool UEnemyRushComponent::TryFireBurst(AActor* TargetActor)
{
	if (!IsValid(OwnerPawn) || !IsValid(TargetActor) || !BurstProjectileClass || BurstProjectileCount <= 0)
	{
		return false;
	}

	const FVector ToTarget = (TargetActor->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal();
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
		SpawnBurstProjectile(SpawnLocation, SpawnRotation, TargetActor);
	}

	return true;
}

void UEnemyRushComponent::SpawnBurstProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation, AActor* TargetActor)
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

	SpawnedProjectile->SetTarget(TargetActor);
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
