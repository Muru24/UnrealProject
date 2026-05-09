#include "SupportFireComponent.h"

#include "EnemyManager.h"
#include "LockOnComponent.h"
#include "SquadCraftActor.h"
#include "SquadComponent.h"

USupportFireComponent::USupportFireComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USupportFireComponent::HandleSupportAutoFire(const TArray<ASquadCraftActor*>& SquadCrafts, ULockOnComponent* LockOnComponent, APawn* InstigatorPawn) const
{
	if (!InstigatorPawn)
	{
		return;
	}

	for (ASquadCraftActor* Craft : SquadCrafts)
	{
		if (!ShouldSupportCraftAutoFire(Craft))
		{
			continue;
		}

		AActor* PreferredTarget = GetSupportAutoFireTargetForCraft(Craft, LockOnComponent);
		if (!PreferredTarget)
		{
			continue;
		}

		Craft->TryAutoFireAt(PreferredTarget->GetActorLocation(), PreferredTarget, InstigatorPawn);
	}
}

AActor* USupportFireComponent::GetSupportAutoFireTargetForCraft(const ASquadCraftActor* Craft, ULockOnComponent* LockOnComponent) const
{
	if (!Craft)
	{
		return nullptr;
	}

	switch (Craft->GetCombatRole())
	{
	case ECraftCombatRole::MainGun:
		return nullptr;

	case ECraftCombatRole::SupportHeavy:
		if (LockOnComponent && LockOnComponent->IsLockOnEnabled() && LockOnComponent->GetCurrentTarget())
		{
			return LockOnComponent->GetCurrentTarget();
		}
		if (AActor* PriorityEnemy = GetPriorityEnemyInRange(Craft ? Craft->GetActorLocation() : FVector::ZeroVector))
		{
			return PriorityEnemy;
		}
		return GetRandomEnemy();

	case ECraftCombatRole::SupportRapid:
	default:
		if (LockOnComponent && LockOnComponent->IsLockOnEnabled() && LockOnComponent->GetCurrentTarget())
		{
			return LockOnComponent->GetCurrentTarget();
		}
		if (AActor* PriorityEnemy = GetPriorityEnemyInRange(Craft->GetActorLocation()))
		{
			return PriorityEnemy;
		}
		return GetRandomEnemy();
	}
}

AActor* USupportFireComponent::GetPriorityEnemyInRange(const FVector& Origin) const
{
	AActor* NearestEnemy = GetNearestEnemy(Origin);
	if (!NearestEnemy)
	{
		return nullptr;
	}

	return FVector::DistSquared(Origin, NearestEnemy->GetActorLocation()) <= FMath::Square(PriorityTargetRange)
		? NearestEnemy
		: nullptr;
}

AActor* USupportFireComponent::GetNearestEnemy(const FVector& Origin) const
{
	if (!GetWorld())
	{
		return nullptr;
	}

	if (UEnemyManager* EnemySubsystem = GetWorld()->GetSubsystem<UEnemyManager>())
	{
		const TArray<APawn*> EnemyList = EnemySubsystem->GetEnemys();
		AActor* BestTarget = nullptr;
		float BestDistanceSq = TNumericLimits<float>::Max();

		for (APawn* Enemy : EnemyList)
		{
			if (!Enemy || Enemy == GetOwner())
			{
				continue;
			}

			const float DistanceSq = FVector::DistSquared(Origin, Enemy->GetActorLocation());
			if (DistanceSq < BestDistanceSq)
			{
				BestDistanceSq = DistanceSq;
				BestTarget = Enemy;
			}
		}

		return BestTarget;
	}

	return nullptr;
}

AActor* USupportFireComponent::GetRandomEnemy() const
{
	if (!GetWorld())
	{
		return nullptr;
	}

	if (UEnemyManager* EnemySubsystem = GetWorld()->GetSubsystem<UEnemyManager>())
	{
		TArray<APawn*> ValidEnemies;
		for (APawn* Enemy : EnemySubsystem->GetEnemys())
		{
			if (IsValid(Enemy) && Enemy != GetOwner())
			{
				ValidEnemies.Add(Enemy);
			}
		}

		if (ValidEnemies.IsEmpty())
		{
			return nullptr;
		}

		return ValidEnemies[FMath::RandRange(0, ValidEnemies.Num() - 1)];
	}

	return nullptr;
}

bool USupportFireComponent::ShouldSupportCraftAutoFire(const ASquadCraftActor* Craft) const
{
	return Craft && !Craft->IsActiveCraft() && Craft->GetCombatRole() != ECraftCombatRole::MainGun;
}
