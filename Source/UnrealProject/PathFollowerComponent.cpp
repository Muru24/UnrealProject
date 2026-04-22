// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFollowerComponent.h"
#include "Components/SplineComponent.h"
#include "StatComponent.h"

UPathFollowerComponent::UPathFollowerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UPathFollowerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (TargetPathActor)
    {
        USplineComponent* Spline = TargetPathActor->FindComponentByClass<USplineComponent>();
        if (Spline)
        {
            CurrentDistance += MoveSpeed * DeltaTime;
            float TotalLength = Spline->GetSplineLength();
            if (CurrentDistance > TotalLength) CurrentDistance = 0.0f;

            FVector NewLoc = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
            FRotator NewRot = Spline->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);

            GetOwner()->SetActorLocationAndRotation(NewLoc, NewRot);
        }
    }
}

void UPathFollowerComponent::BeginPlay()
{
    Super::BeginPlay();

    MoveSpeed = GetOwner()->FindComponentByClass<UStatComponent>()->GetMoveSpeed();
}



