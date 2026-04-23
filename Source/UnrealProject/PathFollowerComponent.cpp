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

    if (!TargetPathActor) return;

    USplineComponent* Spline = TargetPathActor->FindComponentByClass<USplineComponent>();
    if (Spline)
    {
        // 1. 거리 이동 계산
        CurrentDistance += MoveSpeed * DeltaTime;
        float TotalLength = Spline->GetSplineLength();
        if (CurrentDistance > TotalLength) CurrentDistance = 0.0f;

        // 2. 목표 데이터 가져오기 (World Space)
        FVector TargetLoc = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
        FRotator TargetRot = Spline->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);

        // 3. 위치 보간 (VInterp To)
        FVector CurrentLoc = GetOwner()->GetActorLocation();
        FVector SmoothedLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaTime, LocationInterpSpeed);

        // 4. 회전 보간 및 뱅킹(Banking) 계산
        FRotator CurrentRot = GetOwner()->GetActorRotation();

        float YawDelta = FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, TargetRot.Yaw);

        // 뱅킹 강도 조절
        TargetRot.Roll = FMath::Clamp(YawDelta * BankingIntensity, -45.0f, 45.0f); // 최대 45도 제한

        FRotator SmoothedRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RotationInterpSpeed);

        GetOwner()->SetActorLocationAndRotation(SmoothedLoc, SmoothedRot);
    }
}

void UPathFollowerComponent::BeginPlay()
{
    Super::BeginPlay();

    MoveSpeed = GetOwner()->FindComponentByClass<UStatComponent>()->GetMoveSpeed();
}



