// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemyManager.generated.h"

UCLASS()
class UNREALPROJECT_API UEnemyManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UEnemyManager();

	//몬스터 추가
	void AddEnemy(APawn* enemy);

	//몬스터 배열 반환
	TArray<APawn*> GetEnemys() const { return EnemyArr; }
protected:
	UFUNCTION()
	void HandleTargetDestroyed(AActor* DestroyedActor);

	TArray<APawn*> EnemyArr;
};
