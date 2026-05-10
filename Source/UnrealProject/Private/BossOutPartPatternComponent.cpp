#include "BossOutPartPatternComponent.h"

#include "BossOutPart.h"
#include "BulletBase.h"
#include "CraftAttackComponent.h"
#include "HUDManager.h"
#include "LaserAttackComponent.h"
#include "MissileBarrageActor.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealProject/P_Player.h"
#include "SquadCraftActor.h"
#include "SquadRuntimeComponent.h"
#include "TimerManager.h"

UBossOutPartPatternComponent::UBossOutPartPatternComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBossOutPartPatternComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPart = Cast<ABossOutPart>(GetOwner());
	if (!OwnerPart)
	{
		return;
	}

	CommonAttackComponent = OwnerPart->GetCommonAttackComponent();
	LaserAttackComponent = OwnerPart->GetLaserAttackComponent();

	if (CommonAttackComponent)
	{
		CommonAttackComponent->ApplyAttackConfig(CommonAttackConfig);
	}

	ResolvedCommonAttackAimLockDelay = FMath::Max(
		0.0f,
		CommonAttackAimLockDelay +
		CommonAttackAimLockDelayOffset +
		FMath::FRandRange(-CommonAttackAimLockDelayRandomVariance, CommonAttackAimLockDelayRandomVariance));

	if (LaserAttackComponent)
	{
		LaserAttackComponent->SetFireOrigin(OwnerPart->GetSideFireOrigin());
		LaserAttackComponent->SetAttackActiveDuration(LaserDuration);
	}
}

bool UBossOutPartPatternComponent::FireCommonPattern(AActor* TargetActor)
{
	if (!CanStartPattern() || !OwnerPart || !CommonAttackComponent || bCommonAttackPending)
	{
		return false;
	}

	AActor* ResolvedTarget = ResolveTargetActor(TargetActor);
	if (!ResolvedTarget)
	{
		return false;
	}

	OwnerPart->SetUseSideAttackPose(false);
	PendingCommonAttackTargetPoint = ResolvedTarget->GetActorLocation();
	bCommonAttackPending = true;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CommonAttackWindupTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			CommonAttackWindupTimerHandle,
			this,
			&UBossOutPartPatternComponent::ExecutePendingCommonAttack,
			ResolvedCommonAttackAimLockDelay,
			false);
	}

	return true;
}

bool UBossOutPartPatternComponent::ExecuteSpecialPattern(AActor* TargetActor)
{
	if (!CanStartPattern())
	{
		return false;
	}

	switch (PatternType)
	{
	case EBossOutPartPatternType::Laser:
		return ExecuteLaserPattern(TargetActor);
	case EBossOutPartPatternType::SummonAdds:
		return ExecuteSummonPattern();
	case EBossOutPartPatternType::MiniGame:
		return ExecuteMiniGamePattern();
	case EBossOutPartPatternType::HomingMissile:
		return ExecuteHomingMissilePattern(TargetActor);
	default:
		return false;
	}
}

void UBossOutPartPatternComponent::StopActivePattern()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(PatternFinishTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(SummonSequenceTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(CommonAttackWindupTimerHandle);
	}

	if (LaserAttackComponent)
	{
		LaserAttackComponent->StopLaser();
	}

	if (ActiveMissileBarrage)
	{
		ActiveMissileBarrage->Destroy();
		ActiveMissileBarrage = nullptr;
	}

	if (OwnerPart)
	{
		OwnerPart->SetUseSideAttackPose(false);
	}

	PendingSummonSpawnCount = 0;
	bCommonAttackPending = false;
	FinishPattern();
}

bool UBossOutPartPatternComponent::IsTemporarilyDisabled() const
{
	return GetWorld() && GetWorld()->GetTimeSeconds() < DisabledUntilTime;
}

bool UBossOutPartPatternComponent::CanStartPattern() const
{
	return !IsTemporarilyDisabled() && IsValid(OwnerPart) && OwnerPart->IsDissolveInComplete();
}

void UBossOutPartPatternComponent::DisablePatternForDuration(float DisableDuration)
{
	if (DisableDuration <= 0.0f || !GetWorld())
	{
		return;
	}

	DisabledUntilTime = FMath::Max(DisabledUntilTime, GetWorld()->GetTimeSeconds() + DisableDuration);
	StopActivePattern();
}

AActor* UBossOutPartPatternComponent::ResolveTargetActor(AActor* TargetActor) const
{
	if (IsValid(TargetActor))
	{
		return TargetActor;
	}

	AP_Player* PlayerPawn = Cast<AP_Player>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!IsValid(PlayerPawn))
	{
		return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}

	if (USquadRuntimeComponent* SquadRuntimeComponent = PlayerPawn->FindComponentByClass<USquadRuntimeComponent>())
	{
		TArray<ASquadCraftActor*> SquadCrafts;
		SquadRuntimeComponent->GetAllCrafts(SquadCrafts);

		TArray<ASquadCraftActor*> OperationalCrafts;
		for (ASquadCraftActor* Craft : SquadCrafts)
		{
			if (IsValid(Craft) && Craft->IsOperational())
			{
				OperationalCrafts.Add(Craft);
			}
		}

		if (!OperationalCrafts.IsEmpty())
		{
			return OperationalCrafts[FMath::RandRange(0, OperationalCrafts.Num() - 1)];
		}
	}

	return PlayerPawn;
}

void UBossOutPartPatternComponent::BeginPatternLock(float LockDuration)
{
	if (LockDuration <= 0.0f)
	{
		FinishPattern();
		return;
	}

	bPatternActive = true;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(PatternFinishTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			PatternFinishTimerHandle,
			this,
			&UBossOutPartPatternComponent::FinishPattern,
			LockDuration,
			false);
	}
}

void UBossOutPartPatternComponent::FinishPattern()
{
	const bool bWasActive = bPatternActive;
	bPatternActive = false;
	PendingSummonSpawnCount = 0;

	if (OwnerPart)
	{
		OwnerPart->SetUseSideAttackPose(false);
	}

	if (bWasActive)
	{
		OnPatternFinished.Broadcast(this);
	}
}

void UBossOutPartPatternComponent::ExecutePendingCommonAttack()
{
	bCommonAttackPending = false;

	if (!OwnerPart || !CommonAttackComponent || !CanStartPattern())
	{
		return;
	}

	OwnerPart->SetUseSideAttackPose(false);
	CommonAttackComponent->TryAutoFireFromOrigin(
		OwnerPart->GetFrontFireOrigin(),
		PendingCommonAttackTargetPoint,
		nullptr,
		Cast<APawn>(OwnerPart));
}

bool UBossOutPartPatternComponent::ExecuteLaserPattern(AActor* TargetActor)
{
	AActor* ResolvedTarget = ResolveTargetActor(TargetActor);
	if (!OwnerPart || !LaserAttackComponent || !ResolvedTarget)
	{
		return false;
	}

	OwnerPart->SetUseSideAttackPose(true);
	LaserAttackComponent->SetFireOrigin(OwnerPart->GetSideFireOrigin());
	LaserAttackComponent->SetAttackActiveDuration(LaserDuration);
	LaserAttackComponent->StartLaser(ResolvedTarget);
	BeginPatternLock(
		LaserAttackComponent->GetWarningDuration() +
		LaserAttackComponent->GetAttackActiveDuration() +
		LaserRecoveryDuration);
	return true;
}

bool UBossOutPartPatternComponent::ExecuteSummonPattern()
{
	if (!OwnerPart || SummonActorClasses.Num() == 0 || !GetWorld())
	{
		return false;
	}

	OwnerPart->SetUseSideAttackPose(true);
	PendingSummonSpawnCount = SummonCount;

	const float TotalPatternDuration =
		SummonWarningDuration +
		(FMath::Max(0, SummonCount - 1) * SummonSpawnInterval) +
		SummonPatternLockDuration;
	BeginPatternLock(TotalPatternDuration);

	GetWorld()->GetTimerManager().ClearTimer(SummonSequenceTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		SummonSequenceTimerHandle,
		this,
		&UBossOutPartPatternComponent::SpawnNextSummon,
		FMath::Max(0.01f, SummonWarningDuration),
		false);
	return true;
}

bool UBossOutPartPatternComponent::ExecuteMiniGamePattern()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController)
	{
		return false;
	}

	AHUDManager* HUDManager = Cast<AHUDManager>(PlayerController->GetHUD());
	if (!HUDManager)
	{
		return false;
	}

	OwnerPart->SetUseSideAttackPose(true);
	HUDManager->StartTargetMiniGame(MiniGameTargetCount, this);
	BeginPatternLock(MiniGamePatternLockDuration);
	return true;
}

bool UBossOutPartPatternComponent::ExecuteHomingMissilePattern(AActor* TargetActor)
{
	AActor* ResolvedTarget = ResolveTargetActor(TargetActor);
	if (!OwnerPart || !ResolvedTarget || !MissileProjectileClass || !GetWorld())
	{
		return false;
	}

	OwnerPart->SetUseSideAttackPose(true);

	TArray<AActor*> MissileTargets;
	MissileTargets.Reserve(MissileCount);
	for (int32 MissileIndex = 0; MissileIndex < MissileCount; ++MissileIndex)
	{
		MissileTargets.Add(ResolvedTarget);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPart;
	SpawnParams.Instigator = Cast<APawn>(OwnerPart);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector SpawnLocation = OwnerPart->GetSideFireOrigin()
		? OwnerPart->GetSideFireOrigin()->GetComponentLocation()
		: OwnerPart->GetActorLocation();
	const FRotator SpawnRotation = OwnerPart->GetActorRotation();

	ActiveMissileBarrage = GetWorld()->SpawnActor<AMissileBarrageActor>(
		AMissileBarrageActor::StaticClass(),
		SpawnLocation,
		SpawnRotation,
		SpawnParams);

	if (!ActiveMissileBarrage)
	{
		return false;
	}

	ActiveMissileBarrage->AttachToActor(OwnerPart, FAttachmentTransformRules::KeepWorldTransform);
	if (OwnerPart->GetSideFireOrigin())
	{
		ActiveMissileBarrage->AttachToComponent(OwnerPart->GetSideFireOrigin(), FAttachmentTransformRules::KeepWorldTransform);
	}

	ActiveMissileBarrage->ConfigureMissiles(
		MissileMoveSpeed,
		MissileMaxSpeed,
		MissileTurnInterpSpeed,
		MissileHitPoints,
		bMissilesCanBeShotDown,
		MissileInitialFireDelay,
		MissileLaunchSpreadAngleDegrees,
		MissileLaunchPitchSpreadAngleDegrees,
		MissileHomingActivationDelay,
		MissileSpeedRampDelay,
		MissileSpeedRampInterpSpeed);
	ActiveMissileBarrage->Init(MissileTargets, MissileProjectileClass, MissileFireInterval);
	BeginPatternLock(FMath::Max(0.1f, MissileInitialFireDelay + (MissileCount * MissileFireInterval) + 0.25f));
	return true;
}

void UBossOutPartPatternComponent::SpawnNextSummon()
{
	if (!OwnerPart || !GetWorld() || PendingSummonSpawnCount <= 0 || SummonActorClasses.Num() == 0)
	{
		PendingSummonSpawnCount = 0;
		return;
	}

	const int32 SpawnIndex = SummonCount - PendingSummonSpawnCount;
	TSubclassOf<AActor> SpawnClass = SummonActorClasses[FMath::RandRange(0, SummonActorClasses.Num() - 1)];
	if (SpawnClass)
	{
		const USceneComponent* SpawnOriginComponent = OwnerPart->GetSideFireOrigin()
			? OwnerPart->GetSideFireOrigin()
			: OwnerPart->GetRootComponent();

		const FVector SpawnOrigin = SpawnOriginComponent
			? SpawnOriginComponent->GetComponentLocation()
			: OwnerPart->GetActorLocation();
		const FVector ForwardVector = SpawnOriginComponent
			? SpawnOriginComponent->GetForwardVector()
			: OwnerPart->GetActorForwardVector();
		const FVector RightVector = SpawnOriginComponent
			? SpawnOriginComponent->GetRightVector()
			: OwnerPart->GetActorRightVector();
		const FVector UpVector = SpawnOriginComponent
			? SpawnOriginComponent->GetUpVector()
			: OwnerPart->GetActorUpVector();

		const float AngleStepRadians = (SummonCount > 0) ? (2.0f * PI / static_cast<float>(SummonCount)) : 0.0f;
		const float SlotAngleRadians = SpawnIndex * AngleStepRadians;
		const float LateralOffset = (SpawnIndex - ((SummonCount - 1) * 0.5f)) * SummonSpawnLateralSpacing;
		const float ForwardRandomOffset = FMath::FRandRange(-SummonSpawnForwardRandomOffset, SummonSpawnForwardRandomOffset);
		const float LateralRandomOffset = FMath::FRandRange(-SummonSpawnLateralRandomOffset, SummonSpawnLateralRandomOffset);
		const float VerticalRandomOffset = FMath::FRandRange(-SummonSpawnVerticalRandomOffset, SummonSpawnVerticalRandomOffset);
		const FVector RadialOffset =
			(ForwardVector * FMath::Cos(SlotAngleRadians) * SummonSpawnRadius) +
			(RightVector * FMath::Sin(SlotAngleRadians) * SummonSpawnRadius);
		const FVector SpawnLocation =
			SpawnOrigin +
			(ForwardVector * (SummonSpawnForwardDistance + ForwardRandomOffset)) +
			RadialOffset +
			(RightVector * (LateralOffset + LateralRandomOffset)) +
			(UpVector * (SummonSpawnVerticalOffset + VerticalRandomOffset));

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerPart;
		SpawnParams.Instigator = Cast<APawn>(OwnerPart);
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(SpawnClass, SpawnLocation, OwnerPart->GetActorRotation(), SpawnParams);
	}

	--PendingSummonSpawnCount;
	if (PendingSummonSpawnCount > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(
			SummonSequenceTimerHandle,
			this,
			&UBossOutPartPatternComponent::SpawnNextSummon,
			FMath::Max(0.01f, SummonSpawnInterval),
			false);
	}
}
