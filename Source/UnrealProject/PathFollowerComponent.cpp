#include "PathFollowerComponent.h"

#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Path.h"
#include "StatComponent.h"

UPathFollowerComponent::UPathFollowerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPathFollowerComponent::SetAcceleration(float Value)
{
	if (IsAccelerationActive())
	{
		return;
	}
	Acceleration = Value;
	RemainingAccelerationTime = (Value > 0.0f) ? FMath::Max(0.0f, AccelerationDuration) : 0.0f;
}

void UPathFollowerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UStatComponent* OwnerStatComponent = GetOwner()->FindComponentByClass<UStatComponent>())
	{
		const float OwnerMoveSpeed = OwnerStatComponent->GetMoveSpeed();
		if (OwnerMoveSpeed > KINDA_SMALL_NUMBER)
		{
			MoveSpeed = OwnerMoveSpeed;
		}
	}

	if (!TargetPathActor)
	{
		TargetPathActor = UGameplayStatics::GetActorOfClass(GetWorld(), APath::StaticClass());
	}

}

void UPathFollowerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));

	if (!TargetPathActor)
	{
		return;
	}

	USplineComponent* Spline = TargetPathActor->FindComponentByClass<USplineComponent>();
	if (!Spline)
	{
		return;
	}

	const float TotalLength = Spline->GetSplineLength();
	if (TotalLength <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	CurrentDistance += (MoveSpeed + Acceleration) * DeltaTime;

	if (Spline->IsClosedLoop())
	{
		CurrentDistance = FMath::Fmod(CurrentDistance, TotalLength);
	}
	else if (CurrentDistance > TotalLength)
	{
		CurrentDistance = TotalLength;
	}

	const float LookAheadOffset = 200.0f;
	float LookAheadDistance = CurrentDistance + LookAheadOffset;
	if (Spline->IsClosedLoop())
	{
		LookAheadDistance = FMath::Fmod(LookAheadDistance, TotalLength);
	}
	else
	{
		LookAheadDistance = FMath::Clamp(LookAheadDistance, 0.0f, TotalLength);
	}

	const FVector TargetLoc = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	FRotator LookAheadRot = Spline->GetRotationAtDistanceAlongSpline(LookAheadDistance, ESplineCoordinateSpace::World);

	if (LookAheadRot.Pitch < -5.0f)
	{
		LookAheadRot.Pitch *= PitchExaggeration;
	}

	const FRotator CurrentRot = GetOwner()->GetActorRotation();
	const float TargetYawDelta = FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, LookAheadRot.Yaw);

	InterpolatedYawDelta = FMath::FInterpTo(InterpolatedYawDelta, TargetYawDelta, DeltaTime, BankingInterpSpeed);

	const float TargetRoll = FMath::Clamp(InterpolatedYawDelta * BankingIntensity, -60.0f, 60.0f);
	CurrentRoll = FMath::FInterpTo(CurrentRoll, TargetRoll, DeltaTime, BankingInterpSpeed);
	LookAheadRot.Roll = CurrentRoll;

	const FQuat CurrentQuat = FQuat(CurrentRot);
	const FQuat TargetQuat = FQuat(LookAheadRot);
	const FQuat FinalQuat = FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, RotationInterpSpeed);
	const FRotator FinalRot = FinalQuat.Rotator();

	const FVector CurrentLoc = GetOwner()->GetActorLocation();
	const FVector FinalLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaTime, LocationInterpSpeed);

	BaseWorldTransform = FTransform(FinalRot, FinalLoc);
	bHasValidBaseTransform = true;

	if (bApplyOwnerTransform)
	{
		GetOwner()->SetActorLocationAndRotation(FinalLoc, FinalRot);
	}

	if (Acceleration > 0.0f)
	{
		RemainingAccelerationTime = FMath::Max(0.0f, RemainingAccelerationTime - DeltaTime);
		if (RemainingAccelerationTime <= 0.0f)
		{
			Acceleration = 0.0f;
		}
	}
}
