// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeBody.h"
#include "SnakeBodyChargeComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "StatComponent.h"

ASnakeBody::ASnakeBody()
{
	BodyChargeComponent = CreateDefaultSubobject<USnakeBodyChargeComponent>(TEXT("BodyChargeComponent"));
}

void ASnakeBody::BeginPlay()
{
	Super::BeginPlay();

	if (StatComponent)
	{
		StatComponent->OnHpChanged.AddDynamic(this, &ASnakeBody::HandleHpChanged);
	}
}

void ASnakeBody::HandleHpChanged(float CurrentHp)
{
	if (!bSegmentDefeated && CurrentHp <= 0.0f)
	{
		HandleSegmentDefeated();
	}
}

void ASnakeBody::HandleSegmentDefeated()
{
	if (bSegmentDefeated)
	{
		return;
	}

	bSegmentDefeated = true;

	if (BodyChargeComponent)
	{
		BodyChargeComponent->CancelSkillSequence();
		BodyChargeComponent->SetComponentTickEnabled(false);
		BodyChargeComponent->Deactivate();
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
	SetActorTickEnabled(false);

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

void ASnakeBody::OnDissolveOutFinished()
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

