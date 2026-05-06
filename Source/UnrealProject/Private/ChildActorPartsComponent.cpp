#include "ChildActorPartsComponent.h"

#include "Components/ChildActorComponent.h"
#include "EnemyManager.h"

UChildActorPartsComponent::UChildActorPartsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UChildActorPartsComponent::RefreshChildParts()
{
	CachedChildParts.Empty();

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	TArray<UChildActorComponent*> FoundChildParts;
	OwnerActor->GetComponents<UChildActorComponent>(FoundChildParts);

	for (UChildActorComponent* ChildPart : FoundChildParts)
	{
		if (IsValid(ChildPart))
		{
			CachedChildParts.Add(ChildPart);
		}
	}

	if (bRegisterPawnChildrenAsEnemies)
	{
		RegisterPartsToEnemyManager();
	}
}

void UChildActorPartsComponent::GetChildParts(TArray<UChildActorComponent*>& OutChildParts) const
{
	OutChildParts.Reset();
	for (UChildActorComponent* ChildPart : CachedChildParts)
	{
		if (IsValid(ChildPart))
		{
			OutChildParts.Add(ChildPart);
		}
	}
}

void UChildActorPartsComponent::GetChildPartActors(TArray<AActor*>& OutChildPartActors) const
{
	OutChildPartActors.Reset();
	for (UChildActorComponent* ChildPart : CachedChildParts)
	{
		if (IsValid(ChildPart) && IsValid(ChildPart->GetChildActor()))
		{
			OutChildPartActors.Add(ChildPart->GetChildActor());
		}
	}
}

void UChildActorPartsComponent::RegisterPartsToEnemyManager() const
{
	AActor* OwnerActor = GetOwner();
	UWorld* World = OwnerActor ? OwnerActor->GetWorld() : nullptr;
	UEnemyManager* EnemyManager = World ? World->GetSubsystem<UEnemyManager>() : nullptr;
	if (!EnemyManager)
	{
		return;
	}

	for (UChildActorComponent* ChildPart : CachedChildParts)
	{
		if (!IsValid(ChildPart))
		{
			continue;
		}

		if (APawn* ChildPawn = Cast<APawn>(ChildPart->GetChildActor()))
		{
			EnemyManager->AddEnemy(ChildPawn);
		}
	}
}
