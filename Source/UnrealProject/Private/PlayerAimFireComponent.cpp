#include "PlayerAimFireComponent.h"

#include "Engine/World.h"
#include "EnemyManager.h"
#include "LockOnComponent.h"
#include "SquadCraftActor.h"

UPlayerAimFireComponent::UPlayerAimFireComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UPlayerAimFireComponent::FireActiveCraft(APlayerController* PlayerController, ULockOnComponent* LockOnComponent, ASquadCraftActor* ActiveCraft, APawn* InstigatorPawn) const
{
	if (!PlayerController || !ActiveCraft || !InstigatorPawn)
	{
		return false;
	}

	FVector TargetPoint = FVector::ZeroVector;
	AActor* TargetActor = nullptr;
	if (!ResolveAimTarget(PlayerController, LockOnComponent, ActiveCraft, TargetPoint, TargetActor))
	{
		return false;
	}

	return ActiveCraft->FireAt(TargetPoint, TargetActor, InstigatorPawn);
}

bool UPlayerAimFireComponent::TryAutoFireActiveCraft(APlayerController* PlayerController, ULockOnComponent* LockOnComponent, ASquadCraftActor* ActiveCraft, APawn* InstigatorPawn) const
{
	if (!PlayerController || !ActiveCraft || !InstigatorPawn)
	{
		return false;
	}

	FVector TargetPoint = FVector::ZeroVector;
	AActor* TargetActor = nullptr;
	if (!ResolveAimTarget(PlayerController, LockOnComponent, ActiveCraft, TargetPoint, TargetActor))
	{
		return false;
	}

	return ActiveCraft->TryAutoFireAt(TargetPoint, TargetActor, InstigatorPawn);
}

bool UPlayerAimFireComponent::ResolveAimTarget(APlayerController* PlayerController, ULockOnComponent* LockOnComponent, ASquadCraftActor* ActiveCraft, FVector& OutTargetPoint, AActor*& OutTargetActor) const
{
	OutTargetPoint = FVector::ZeroVector;
	OutTargetActor = nullptr;

	if (LockOnComponent && LockOnComponent->IsLockOnEnabled() && LockOnComponent->GetCurrentTarget())
	{
		OutTargetPoint = LockOnComponent->GetCurrentTarget()->GetActorLocation();
		OutTargetActor = LockOnComponent->GetCurrentTarget();
		return true;
	}

	FVector MouseWorldOrigin = FVector::ZeroVector;
	FVector MouseWorldDirection = FVector::ForwardVector;
	if (PlayerController->DeprojectMousePositionToWorld(MouseWorldOrigin, MouseWorldDirection))
	{
		OutTargetPoint = MouseWorldOrigin + (MouseWorldDirection * MouseAimDistance);
		OutTargetActor = nullptr;
		return true;
	}

	const FVector SearchOrigin = ActiveCraft ? ActiveCraft->GetActorLocation() : (GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector);
	if (AActor* PriorityEnemy = GetPriorityEnemyInRange(SearchOrigin))
	{
		OutTargetPoint = PriorityEnemy->GetActorLocation();
		OutTargetActor = PriorityEnemy;
		return true;
	}

	if (AActor* RandomEnemy = GetRandomEnemy())
	{
		OutTargetPoint = RandomEnemy->GetActorLocation();
		OutTargetActor = RandomEnemy;
		return true;
	}

	return false;
}

AActor* UPlayerAimFireComponent::GetPriorityEnemyInRange(const FVector& Origin) const
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

AActor* UPlayerAimFireComponent::GetNearestEnemy(const FVector& Origin) const
{
	if (!GetWorld())
	{
		return nullptr;
	}

	if (UEnemyManager* EnemySubsystem = GetWorld()->GetSubsystem<UEnemyManager>())
	{
		AActor* BestTarget = nullptr;
		float BestDistanceSq = TNumericLimits<float>::Max();

		for (APawn* Enemy : EnemySubsystem->GetEnemys())
		{
			if (IsValid(Enemy) && Enemy != GetOwner())
			{
				const float DistanceSq = FVector::DistSquared(Origin, Enemy->GetActorLocation());
				if (DistanceSq < BestDistanceSq)
				{
					BestDistanceSq = DistanceSq;
					BestTarget = Enemy;
				}
			}
		}

		return BestTarget;
	}

	return nullptr;
}

AActor* UPlayerAimFireComponent::GetRandomEnemy() const
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
