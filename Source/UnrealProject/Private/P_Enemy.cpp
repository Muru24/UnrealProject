// Fill out your copyright notice in the Description page of Project Settings.


#include "P_Enemy.h"
#include "EnemyRushComponent.h"
#include "EnemyManager.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "StatComponent.h"

AP_Enemy::AP_Enemy()
{
    EnemyRushComponent = CreateDefaultSubobject<UEnemyRushComponent>(TEXT("EnemyRushComp"));
}

void AP_Enemy::BeginPlay()
{
    APawn_Template::BeginPlay();

    if (GetWorld())
    {
        GetWorld()->GetSubsystem<UEnemyManager>()->AddEnemy(this);
    }

    if (StatComponent)
    {
        StatComponent->OnHpChanged.AddDynamic(this, &AP_Enemy::HandleHpChanged);
    }
}

void AP_Enemy::HandleHpChanged(float CurrentHp)
{
    if (!bEnemyDefeated && CurrentHp <= 0.0f)
    {
        HandleEnemyDefeated();
    }
}

void AP_Enemy::HandleEnemyDefeated()
{
	if (bEnemyDefeated)
	{
        return;
    }

    bEnemyDefeated = true;

    if (EnemyRushComponent)
    {
        EnemyRushComponent->SetComponentTickEnabled(false);
        EnemyRushComponent->Deactivate();
    }

    if (CollisionComponent)
    {
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CollisionComponent->SetGenerateOverlapEvents(false);
    }

	if (PlaneMesh)
	{
		PlaneMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetActorEnableCollision(false);

	if (bHasDissolveParameter && DynamicMaterial)
	{
		StartDissolveOut();
	}
	else if (DestroyDelay > 0.0f)
	{
		SetLifeSpan(DestroyDelay);
	}
	else
	{
		Destroy();
	}
}

void AP_Enemy::OnDissolveOutFinished()
{
	Super::OnDissolveOutFinished();

	if (DestroyDelay > 0.0f)
	{
		SetLifeSpan(DestroyDelay);
	}
	else
	{
		Destroy();
	}
}
