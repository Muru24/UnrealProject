// Fill out your copyright notice in the Description page of Project Settings.


#include "LockOnComponent.h"
#include "EnemyManager.h"

ULockOnComponent::ULockOnComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	EnemyManager = GetWorld()->GetSubsystem<UEnemyManager>();
}


void ULockOnComponent::TraceTarget()
{
	isLockOn = !isLockOn;

	if (!isLockOn) return;
	if (!EnemyManager) return;

	const TArray<APawn*>& EnemyList = EnemyManager->GetEnemys();
	FVector StartPos = GetOwner()->GetActorLocation();
	float min = 99999;
	targetIndex = 0;
	if (!EnemyList.IsEmpty())
	{
		for (APawn* Enemy : EnemyList) {
			targetIndex++;
			float Dist = FVector::Dist(StartPos, Enemy->GetActorLocation());
			if (Dist < min)
			{
				min = Dist;
				target = Enemy;
			}
		}
	}
}


void ULockOnComponent::ChangeTarget()
{
	if (!isLockOn) return;
	if (!EnemyManager) return;

	const TArray<APawn*>& EnemyList = EnemyManager->GetEnemys();

	int32 Size = EnemyList.Num();
	if (Size < 2) return;
	
	if (targetIndex + 1 >= Size)
	{
		targetIndex = 0;
	}
	else targetIndex++;

	target = EnemyList[targetIndex];
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

