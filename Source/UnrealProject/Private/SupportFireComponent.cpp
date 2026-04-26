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
		return GetNearestEnemy(GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector);

	case ECraftCombatRole::SupportRapid:
	default:
		return GetNearestEnemy(Craft->GetActorLocation());
	}
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

bool USupportFireComponent::ShouldSupportCraftAutoFire(const ASquadCraftActor* Craft) const
{
	return Craft && !Craft->IsActiveCraft() && Craft->GetCombatRole() != ECraftCombatRole::MainGun;
}
