#include "BossOutPartPatternComponent.h"

#include "BossOutPart.h"
#include "BulletBase.h"
#include "CraftAttackComponent.h"
#include "HUDManager.h"
#include "LaserAttackComponent.h"
#include "MissileBarrageActor.h"
#include "Kismet/GameplayStatics.h"
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

	if (LaserAttackComponent)
	{
		LaserAttackComponent->SetFireOrigin(OwnerPart->GetSideFireOrigin());
		LaserAttackComponent->SetAttackActiveDuration(LaserDuration);
	}
}

bool UBossOutPartPatternComponent::FireCommonPattern(AActor* TargetActor)
{
	if (!OwnerPart || !CommonAttackComponent)
	{
		return false;
	}

	AActor* ResolvedTarget = ResolveTargetActor(TargetActor);
	if (!ResolvedTarget)
	{
		return false;
	}

	OwnerPart->SetUseSideAttackPose(false);
	return CommonAttackComponent->TryAutoFireFromOrigin(
		OwnerPart->GetFrontFireOrigin(),
		ResolvedTarget->GetActorLocation(),
		ResolvedTarget,
		Cast<APawn>(OwnerPart));
}

bool UBossOutPartPatternComponent::ExecuteSpecialPattern(AActor* TargetActor)
{
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

	FinishPattern();
}

AActor* UBossOutPartPatternComponent::ResolveTargetActor(AActor* TargetActor) const
{
	if (IsValid(TargetActor))
	{
		return TargetActor;
	}

	return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
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

	if (OwnerPart)
	{
		OwnerPart->SetUseSideAttackPose(false);
	}

	if (bWasActive)
	{
		OnPatternFinished.Broadcast(this);
	}
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

	bool bSpawnedAny = false;
	const FVector SpawnOrigin = OwnerPart->GetSideFireOrigin()
		? OwnerPart->GetSideFireOrigin()->GetComponentLocation()
		: OwnerPart->GetActorLocation();

	for (int32 SpawnIndex = 0; SpawnIndex < SummonCount; ++SpawnIndex)
	{
		TSubclassOf<AActor> SpawnClass = SummonActorClasses[FMath::RandRange(0, SummonActorClasses.Num() - 1)];
		if (!SpawnClass)
		{
			continue;
		}

		const FVector SpawnLocation = SpawnOrigin + FVector(0.0f, 120.0f * SpawnIndex, 0.0f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerPart;
		SpawnParams.Instigator = Cast<APawn>(OwnerPart);
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		bSpawnedAny |= (GetWorld()->SpawnActor<AActor>(SpawnClass, SpawnLocation, OwnerPart->GetActorRotation(), SpawnParams) != nullptr);
	}

	if (bSpawnedAny)
	{
		BeginPatternLock(SummonPatternLockDuration);
	}

	return bSpawnedAny;
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
	HUDManager->StartTargetMiniGame(MiniGameTargetCount);
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

	ActiveMissileBarrage->Init(MissileTargets, MissileProjectileClass, MissileFireInterval);
	BeginPatternLock(FMath::Max(0.1f, MissileCount * MissileFireInterval + 0.25f));
	return true;
}
