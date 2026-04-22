// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"

UEnemyManager::UEnemyManager()
{
}

void UEnemyManager::AddEnemy(APawn* enemy)
{
	if (enemy)
	{
		EnemyArr.Add(enemy);
		enemy->OnDestroyed.AddDynamic(this, &UEnemyManager::HandleTargetDestroyed);
	}

}

void UEnemyManager::HandleTargetDestroyed(AActor* DestroyedActor)
{
	if (APawn* DeadPawn = Cast<APawn>(DestroyedActor))
	{
		EnemyArr.Remove(DeadPawn);
	}
}
