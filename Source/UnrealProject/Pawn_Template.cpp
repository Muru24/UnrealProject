// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawn_Template.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PathFollowerComponent.h"
#include "BulletBase.h"

#include "DrawDebugHelpers.h"

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

	PathFollower = CreateDefaultSubobject<UPathFollowerComponent>(TEXT("PathFollowerComp"));
}

void APawn_Template::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void APawn_Template::Initialize_GameManager_Pawn()
{

}

void APawn_Template::BeginPlay()
{
	Super::BeginPlay();

	TArray<USceneComponent*> Components;
	GetComponents<USceneComponent>(Components);

	for (USceneComponent* Comp : Components)
	{
		if (Comp->GetName().Contains(TEXT("FirePos")))
		{
			FirePoints.Add(Comp);
		}
	}

	if (PathFollower)
		PathFollower->SetPathSpeed(PawnState.move.MoveSpeed);

}

void APawn_Template::Fire()
{

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !BulletBase) return;

	//마우스의 월드위치와 방향 가져오기
	FVector MouseLocation, MouseDirection;
	if (!PC->DeprojectMousePositionToWorld(MouseLocation, MouseDirection)) return;

	FVector TargetPoint = MouseLocation + (MouseDirection * 10000.f);

	for (USceneComponent* Point : FirePoints)
	{
		if (!Point) continue;

		FVector SpawnLocation = Point->GetComponentLocation();

		FVector LookAtDir = (TargetPoint - SpawnLocation).GetSafeNormal();
		FRotator LookAtRotation = LookAtDir.Rotation();

		Point->SetWorldRotation(LookAtRotation);

		ABulletBase* NewBullet = GetWorld()->SpawnActor<ABulletBase>(BulletBase, SpawnLocation, LookAtRotation);

		if (NewBullet)
		{
			NewBullet->InitBullet(LookAtDir);
		}
	}
}



