#include "RailOffsetComponent.h"

URailOffsetComponent::URailOffsetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URailOffsetComponent::SetHorizontalInput(float Value)
{
	CurrentInput.X = FMath::Clamp(Value, -1.0f, 1.0f);
}

void URailOffsetComponent::SetVerticalInput(float Value)
{
	CurrentInput.Y = FMath::Clamp(Value, -1.0f, 1.0f);
}

void URailOffsetComponent::UpdateOffset(float DeltaTime)
{
	const FVector2D PreviousOffset = CurrentOffset;
	const bool bHasHorizontalInput = !FMath::IsNearlyZero(CurrentInput.X);
	const bool bHasVerticalInput = !FMath::IsNearlyZero(CurrentInput.Y);
	const float TargetHorizontalOffset = CurrentInput.X * MaxHorizontalOffset;
	const float TargetVerticalOffset = CurrentInput.Y * MaxVerticalOffset;

	CurrentOffset.X = FMath::FInterpTo(
		CurrentOffset.X,
		TargetHorizontalOffset,
		DeltaTime,
		bHasHorizontalInput ? HorizontalInputInterpSpeed : HorizontalReturnInterpSpeed);

	CurrentOffset.Y = FMath::FInterpTo(
		CurrentOffset.Y,
		TargetVerticalOffset,
		DeltaTime,
		bHasVerticalInput ? VerticalInputInterpSpeed : VerticalReturnInterpSpeed);

	CurrentOffset.X = FMath::Clamp(CurrentOffset.X, -MaxHorizontalOffset, MaxHorizontalOffset);
	CurrentOffset.Y = FMath::Clamp(CurrentOffset.Y, -MaxVerticalOffset, MaxVerticalOffset);

	if (DeltaTime > KINDA_SMALL_NUMBER)
	{
		OffsetVelocity = (CurrentOffset - PreviousOffset) / DeltaTime;
	}
	else
	{
		OffsetVelocity = FVector2D::ZeroVector;
	}

	const float HorizontalSpeedRatio = FMath::Clamp(
		HorizontalMoveSpeed > 0.0f ? OffsetVelocity.X / HorizontalMoveSpeed : 0.0f,
		-1.0f,
		1.0f);
	const float VerticalSpeedRatio = FMath::Clamp(
		VerticalMoveSpeed > 0.0f ? OffsetVelocity.Y / VerticalMoveSpeed : 0.0f,
		-1.0f,
		1.0f);

	const FRotator TargetTiltRotation(
		-VerticalSpeedRatio * MaxPitchTilt,
		0.0f,
		-HorizontalSpeedRatio * MaxRollTilt);

	VisualTiltRotation = FMath::RInterpTo(VisualTiltRotation, TargetTiltRotation, DeltaTime, TiltResponseSpeed);
}
