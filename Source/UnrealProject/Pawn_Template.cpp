// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawn_Template.h"
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

}

void APawn_Template::BeginPlay()
{
	Super::BeginPlay();
}




