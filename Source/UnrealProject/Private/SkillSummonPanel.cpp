// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSummonPanel.h"

#include "EnemyManager.h"
#include "Funnel.h"
#include "LockOnComponent.h"
#include "Engine/World.h"

bool USkillSummonPanel::ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration)
{
	if (!Super::ActivateSkill_Implementation(SourceActor, TargetActor, SkillDuration) || !IsValid(SourceActor))
	{
		return false;
	}

	AActor* ResolvedTarget = ResolveTargetActor(SourceActor, TargetActor);
	if (!IsValid(ResolvedTarget))
	{
		return false;
	}

	UWorld* World = SourceActor->GetWorld();
	if (!World)
	{
		return false;
	}

	TSubclassOf<AFunnel> SpawnClass = FunnelClass;
	if (!SpawnClass)
	{
		SpawnClass = AFunnel::StaticClass();
	}
	const int32 SpawnCount = FMath::Max(1, FunnelCount);

	for (int32 Index = 0; Index < SpawnCount; ++Index)
	{
		const float AngleRadians = (static_cast<float>(Index) / static_cast<float>(SpawnCount)) * UE_TWO_PI;
		const FVector RadialOffset(
			FMath::Cos(AngleRadians) * FunnelSpawnRadius,
			FMath::Sin(AngleRadians) * FunnelSpawnRadius,
			0.0f);
		const FVector SpawnLocation = SourceActor->GetActorLocation() + FunnelSpawnOffset + RadialOffset;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = SourceActor;
		SpawnParams.Instigator = Cast<APawn>(SourceActor);
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AFunnel* SpawnedFunnel = World->SpawnActor<AFunnel>(
			SpawnClass,
			SpawnLocation,
			SourceActor->GetActorRotation(),
			SpawnParams);

		if (!SpawnedFunnel)
		{
			continue;
		}

		SpawnedFunnel->SetBeamTarget(ResolvedTarget);
		SpawnedFunnel->StartBeamAttack(ResolvedTarget);
	}

	return true;
}

AActor* USkillSummonPanel::ResolveTargetActor(AActor* SourceActor, AActor* RequestedTarget) const
{
	if (IsValid(RequestedTarget))
	{
		return RequestedTarget;
	}

	if (AActor* LockOnTarget = ResolveLockOnTarget(SourceActor))
	{
		return LockOnTarget;
	}

	return ResolveNearestEnemy(SourceActor);
}

AActor* USkillSummonPanel::ResolveLockOnTarget(AActor* SourceActor) const
{
	if (!IsValid(SourceActor))
	{
		return nullptr;
	}

	AActor* SearchActor = SourceActor;
	while (SearchActor)
	{
		if (ULockOnComponent* LockOnComponent = SearchActor->FindComponentByClass<ULockOnComponent>())
		{
			if (APawn* CurrentTarget = LockOnComponent->GetCurrentTarget())
			{
				return CurrentTarget;
			}
		}

		SearchActor = SearchActor->GetOwner();
	}

	return nullptr;
}

AActor* USkillSummonPanel::ResolveNearestEnemy(AActor* SourceActor) const
{
	if (!IsValid(SourceActor))
	{
		return nullptr;
	}

	UWorld* World = SourceActor->GetWorld();
	UEnemyManager* EnemyManager = World ? World->GetSubsystem<UEnemyManager>() : nullptr;
	if (!EnemyManager)
	{
		return nullptr;
	}

	const TArray<APawn*>& EnemyList = EnemyManager->GetEnemys();
	AActor* NearestEnemy = nullptr;
	float BestDistanceSquared = TNumericLimits<float>::Max();

	for (APawn* EnemyPawn : EnemyList)
	{
		if (!IsValid(EnemyPawn))
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared(SourceActor->GetActorLocation(), EnemyPawn->GetActorLocation());
		if (DistanceSquared < BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;
			NearestEnemy = EnemyPawn;
		}
	}

	return NearestEnemy;
}
