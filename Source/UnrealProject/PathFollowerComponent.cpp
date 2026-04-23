// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFollowerComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Path.h"
#include "StatComponent.h"

UPathFollowerComponent::UPathFollowerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPathFollowerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UStatComponent* OwnerStatComponent = GetOwner()->FindComponentByClass<UStatComponent>())
	{
		MoveSpeed = OwnerStatComponent->GetMoveSpeed();
	}

	if (!TargetPathActor)
	{
		TargetPathActor = UGameplayStatics::GetActorOfClass(GetWorld(), APath::StaticClass());
	}
}

void UPathFollowerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!TargetPathActor) return;
	USplineComponent* Spline = TargetPathActor->FindComponentByClass<USplineComponent>();
	if (!Spline) return;

	float TotalLength = Spline->GetSplineLength();
	CurrentDistance += MoveSpeed * DeltaTime;

	// 1. 루핑(Looping) 및 거리 처리 개선
	if (Spline->IsClosedLoop())
	{
		CurrentDistance = FMath::Fmod(CurrentDistance, TotalLength);
	}
	else if (CurrentDistance > TotalLength)
	{
		CurrentDistance = 0.0f; // 루프가 아닐 경우 처음으로 리셋
	}

	// 2. Look-ahead 거리 계산 (루핑 고려)
	float LookAheadOffset = 200.0f;
	float LookAheadDistance = CurrentDistance + LookAheadOffset;
	if (Spline->IsClosedLoop())
	{
		LookAheadDistance = FMath::Fmod(LookAheadDistance, TotalLength);
	}
	else
	{
		LookAheadDistance = FMath::Clamp(LookAheadDistance, 0.0f, TotalLength);
	}

	FVector TargetLoc = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	FRotator LookAheadRot = Spline->GetRotationAtDistanceAlongSpline(LookAheadDistance, ESplineCoordinateSpace::World);

	// Pitch 강조 (하강 시 더 가파르게)
	if (LookAheadRot.Pitch < -5.0f)
	{
		LookAheadRot.Pitch *= PitchExaggeration;
	}

	FRotator CurrentRot = GetOwner()->GetActorRotation();

	// 3. 뱅킹(Banking) 로직 개선 (정적 변수 제거 및 보간 강화)
	float TargetYawDelta = FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, LookAheadRot.Yaw);
	
	// Yaw 변화량을 부드럽게 추적
	InterpolatedYawDelta = FMath::FInterpTo(InterpolatedYawDelta, TargetYawDelta, DeltaTime, BankingInterpSpeed);
	
	// 타겟 롤 계산 및 부드러운 적용
	float TargetRoll = FMath::Clamp(InterpolatedYawDelta * BankingIntensity, -60.0f, 60.0f);
	CurrentRoll = FMath::FInterpTo(CurrentRoll, TargetRoll, DeltaTime, BankingInterpSpeed);
	LookAheadRot.Roll = CurrentRoll;

	// 4. 회전 보간 (쿼터니언 Slerp를 사용하여 짐벌 락 방지 및 부드러움 확보)
	FQuat CurrentQuat = FQuat(CurrentRot);
	FQuat TargetQuat = FQuat(LookAheadRot);
	FQuat FinalQuat = FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, RotationInterpSpeed);
	FRotator FinalRot = FinalQuat.Rotator();

	// 5. 위치 보간
	FVector CurrentLoc = GetOwner()->GetActorLocation();
	FVector FinalLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaTime, LocationInterpSpeed);

	BaseWorldTransform = FTransform(FinalRot, FinalLoc);
	bHasValidBaseTransform = true;

	if (bApplyOwnerTransform)
	{
		GetOwner()->SetActorLocationAndRotation(FinalLoc, FinalRot);
	}
}
