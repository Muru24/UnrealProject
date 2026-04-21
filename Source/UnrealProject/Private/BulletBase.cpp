// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBase.h"

ABulletBase::ABulletBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABulletBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    UpdateStraightMovement(DeltaTime);
    HandleLifeTime(DeltaTime);

}

void ABulletBase::UpdateStraightMovement(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    FVector NextLocation = CurrentLocation + (MoveDirection * BulletSpeed * DeltaTime);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CurrentLocation, NextLocation, ECC_Visibility, Params);

    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        FString ActorName = HitActor ? HitActor->GetName() : TEXT("None");

        UE_LOG(LogTemp, Warning, TEXT("Bullet Hit: %s"), *ActorName);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Hit: %s"), *ActorName));
        }

        SetActorLocation(HitResult.Location);
        Destroy();
    }
    else
    {
        SetActorLocation(NextLocation);
    }
}

void ABulletBase::HandleLifeTime(float DeltaTime)
{
    LifeTime -= DeltaTime;
    if (LifeTime <= 0.0f)
    {
        Destroy();
    }
}

void ABulletBase::InitBullet(FVector Direction)
{
	MoveDirection = Direction.GetSafeNormal();
}

