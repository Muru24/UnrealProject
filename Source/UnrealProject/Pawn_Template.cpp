// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawn_Template.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PathFollowerComponent.h"
#include "StatComponent.h"

APawn_Template::APawn_Template()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetSphereRadius(100.f);
	CollisionComponent->SetCollisionProfileName(TEXT("Pawn"));

	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
	PlaneMesh->SetupAttachment(RootComponent);
	PlaneMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	
}

void APawn_Template::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasDissolveParameter && DynamicMaterial)
	{
		if (bIsDissolvingIn)
		{
			CurrentDissolvePower = FMath::FInterpConstantTo(CurrentDissolvePower, -1.0f, DeltaTime, DissolveSpeed);
			DynamicMaterial->SetScalarParameterValue(TEXT("DissovePower"), CurrentDissolvePower);
			if (CurrentDissolvePower <= -1.0f)
			{
				bIsDissolvingIn = false;
			}
		}
		else if (bIsDissolvingOut)
		{
			CurrentDissolvePower = FMath::FInterpConstantTo(CurrentDissolvePower, 1.0f, DeltaTime, DissolveSpeed);
			DynamicMaterial->SetScalarParameterValue(TEXT("DissovePower"), CurrentDissolvePower);
			if (CurrentDissolvePower >= 1.0f)
			{
				bIsDissolvingOut = false;
				OnDissolveOutFinished();
			}
		}
	}
}

void APawn_Template::BeginPlay()
{
	Super::BeginPlay();

	if (PlaneMesh)
	{
		DynamicMaterial = PlaneMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynamicMaterial)
		{
			float DefaultValue = 0.0f;
			if (DynamicMaterial->GetScalarParameterValue(TEXT("DissovePower"), DefaultValue))
			{
				bHasDissolveParameter = true;
				CurrentDissolvePower = 1.0f;
				DynamicMaterial->SetScalarParameterValue(TEXT("DissovePower"), CurrentDissolvePower);
				bIsDissolvingIn = true;
			}
			else
			{
				bHasDissolveParameter = false;
			}
		}
	}
}

void APawn_Template::OnDissolveOutFinished()
{
	// 기본 구현: 파생 클래스에서 오버라이드 가능
}




