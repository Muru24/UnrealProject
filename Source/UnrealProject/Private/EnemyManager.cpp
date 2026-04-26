#include "EnemyManager.h"

UEnemyManager::UEnemyManager()
{
}

void UEnemyManager::AddEnemy(APawn* Enemy)
{
	if (!Enemy || EnemyArr.Contains(Enemy))
	{
		return;
	}

	EnemyArr.Add(Enemy);
	Enemy->OnDestroyed.AddDynamic(this, &UEnemyManager::HandleTargetDestroyed);
}

void UEnemyManager::HandleTargetDestroyed(AActor* DestroyedActor)
{
	if (APawn* DeadPawn = Cast<APawn>(DestroyedActor))
	{
		EnemyArr.Remove(DeadPawn);
	}
}
