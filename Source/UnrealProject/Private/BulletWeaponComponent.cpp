// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletWeaponComponent.h"
#include "BulletBase.h"

UBulletWeaponComponent::UBulletWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UBulletWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<USceneComponent*> AllComps;
	GetOwner()->GetComponents<USceneComponent>(AllComps);

	for (USceneComponent* Comp : AllComps)
	{
		if (Comp->GetName().Contains(TEXT("FirePos")))
		{
			FirePoints.Add(Comp);
		}
	}
}


void UBulletWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UBulletWeaponComponent::Fire(FVector TargetPoint, AActor* TargetActor)
{
	for (USceneComponent* Point : FirePoints)
	{
		if (!Point) continue;

		FVector SpawnLocation = Point->GetComponentLocation();

		FVector LookAtDir = (TargetPoint - SpawnLocation).GetSafeNormal();
		FRotator LookAtRotation = LookAtDir.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Cast<APawn>(GetOwner());

		ABulletBase* NewBullet = GetWorld()->SpawnActor<ABulletBase>(Prefab, SpawnLocation, LookAtRotation, SpawnParams);

		if (NewBullet && TargetActor)
		{
			NewBullet->SetTarget(TargetActor);
		}
	}
}

