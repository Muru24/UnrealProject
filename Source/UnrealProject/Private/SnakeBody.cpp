// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeBody.h"
#include "SnakeBodyChargeComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Snake_CompositeMaster.h"
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
	SetActorTickEnabled(true);

	if (UChildActorComponent* ParentChildActorComponent = GetParentComponent())
	{
		if (ASnake_CompositeMaster* OwningSnakeMaster = Cast<ASnake_CompositeMaster>(ParentChildActorComponent->GetOwner()))
		{
			OwningSnakeMaster->UnregisterSegment(ParentChildActorComponent);
		}
	}

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

	if (UChildActorComponent* ParentChildActorComponent = GetParentComponent())
	{
		ParentChildActorComponent->SetChildActorClass(nullptr);
	}

	if (DestroyDelay > 0.0f)
	{
		SetLifeSpan(DestroyDelay);
	}
	else
	{
		Destroy();
	}
}

