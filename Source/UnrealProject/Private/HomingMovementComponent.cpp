// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingMovementComponent.h"
#include "BulletBase.h"
#include "UObject/UObjectGlobals.h"

UHomingMovementComponent::UHomingMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UHomingMovementComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UHomingMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    ABulletBase* BulletOwner = Cast<ABulletBase>(GetOwner());
    if (!BulletOwner || BulletOwner->IsPendingKill()) return;

    AActor* CurrentTarget = BulletOwner->GetTarget();

    if (CurrentTarget && !IsValid(CurrentTarget))
    {
        BulletOwner->SetTarget(nullptr);
        CurrentTarget = nullptr;
    }

    if (CurrentTarget)
    {
        FVector Direction = (CurrentTarget->GetActorLocation() - BulletOwner->GetActorLocation()).GetSafeNormal();
        FRotator TargetRot = Direction.Rotation();

        FRotator NewRot = FMath::RInterpTo(BulletOwner->GetActorRotation(), TargetRot, DeltaTime, 20.0f);
        BulletOwner->SetActorRotation(NewRot);
    }

    FVector MoveStep = BulletOwner->GetActorForwardVector() * BulletOwner->GetSpeed() * DeltaTime;
    FHitResult SweepHit;

    BulletOwner->AddActorWorldOffset(MoveStep, true, &SweepHit);

    
}

