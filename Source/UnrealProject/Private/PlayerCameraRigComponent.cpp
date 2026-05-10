#include "PlayerCameraRigComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "SquadCraftActor.h"

UPlayerCameraRigComponent::UPlayerCameraRigComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerCameraRigComponent::UpdateCameraPan(APlayerController* PlayerController, USpringArmComponent* SpringArmComponent, float DeltaTime, bool bSuppressPan) const
{
	if (!PlayerController || !SpringArmComponent)
	{
		return;
	}

	if (bSuppressPan)
	{
		SpringArmComponent->SetRelativeRotation(
			FMath::RInterpTo(SpringArmComponent->GetRelativeRotation(), FRotator::ZeroRotator, DeltaTime, CameraMoveSpeed));
		return;
	}

	int32 ViewportSizeX = 0;
	int32 ViewportSizeY = 0;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!PlayerController->GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	float RangeX = (MouseX / (ViewportSizeX * 0.5f)) - 1.0f;
	float RangeY = (MouseY / (ViewportSizeY * 0.5f)) - 1.0f;

	if (FMath::Abs(RangeX) < MouseDeadZone)
	{
		RangeX = 0.0f;
	}

	if (RangeY > 0.0f)
	{
		RangeY = 0.0f;
	}

	RangeX = FMath::Clamp(RangeX, -1.0f, 1.0f);
	RangeY = FMath::Clamp(RangeY, -1.0f, 1.0f);

	const float MaxRotationAngle = 12.0f;
	const FRotator TargetRotation(-RangeY * MaxRotationAngle, RangeX * MaxRotationAngle, 0.0f);

	SpringArmComponent->SetRelativeRotation(
		FMath::RInterpTo(SpringArmComponent->GetRelativeRotation(), TargetRotation, DeltaTime, CameraMoveSpeed));
}

void UPlayerCameraRigComponent::UpdateCameraAnchor(USpringArmComponent* SpringArmComponent, ASquadCraftActor* ActiveCraft, bool bSnapToTarget) const
{
	if (!SpringArmComponent)
	{
		return;
	}

	const FVector CraftLocation = ActiveCraft ? ActiveCraft->GetCurrentRelativeLocation() : FVector::ZeroVector;
	if (!bHasCachedBaseArmLocation)
	{
		CachedBaseArmLocation = SpringArmComponent->GetRelativeLocation();
		bHasCachedBaseArmLocation = true;
	}

	const FVector AnchorOffset(
		CraftLocation.X * CameraAnchorInfluence.X,
		CraftLocation.Y * CameraAnchorInfluence.Y,
		CraftLocation.Z * CameraAnchorInfluence.Z);
	const FVector TargetLocation = CachedBaseArmLocation + AnchorOffset;

	if (bSnapToTarget)
	{
		SpringArmComponent->SetRelativeLocation(TargetLocation);
		return;
	}

	const UWorld* World = GetWorld();
	const float DeltaTime = World ? World->GetDeltaSeconds() : 0.0f;
	const FVector NewLocation = FMath::VInterpTo(
		SpringArmComponent->GetRelativeLocation(),
		TargetLocation,
		DeltaTime,
		CameraAnchorFollowSpeed);

	SpringArmComponent->SetRelativeLocation(NewLocation);
}

void UPlayerCameraRigComponent::UpdateCameraZoom(USpringArmComponent* SpringArmComponent, bool bIsAccelerating, float DeltaTime) const
{
	if (!SpringArmComponent)
	{
		return;
	}

	if (CachedBaseArmLength < 0.0f)
	{
		CachedBaseArmLength = SpringArmComponent->TargetArmLength;
	}

	const float BaseArmLength = CachedBaseArmLength;
	const float TargetArmLength = bIsAccelerating
		? BaseArmLength + AccelerationZoomOutDistance
		: BaseArmLength;
	const float ZoomInterpSpeed = bIsAccelerating ? FMath::Max(1.0f, CameraZoomInterpSpeed) : FMath::Max(1.0f, CameraZoomReturnInterpSpeed);

	SpringArmComponent->TargetArmLength = FMath::FInterpTo(
		SpringArmComponent->TargetArmLength,
		TargetArmLength,
		DeltaTime,
		ZoomInterpSpeed);
}

void UPlayerCameraRigComponent::UpdateCameraShake(USpringArmComponent* SpringArmComponent, float DeltaTime) const
{
	if (!SpringArmComponent)
	{
		return;
	}

	if (!bHasCachedBaseSocketOffset)
	{
		CachedBaseSocketOffset = SpringArmComponent->SocketOffset;
		bHasCachedBaseSocketOffset = true;
	}

	if (CurrentShakeRemainingTime > 0.0f && CurrentShakeDuration > 0.0f)
	{
		CurrentShakeElapsedTime += DeltaTime;
		CurrentShakeRemainingTime = FMath::Max(0.0f, CurrentShakeRemainingTime - DeltaTime);

		const float NormalizedStrength = FMath::Clamp(CurrentShakeRemainingTime / CurrentShakeDuration, 0.0f, 1.0f);
		const float Frequency = FMath::Max(1.0f, CameraShakeFrequency);
		const FVector Oscillation(
			FMath::Sin(CurrentShakeElapsedTime * Frequency * 1.31f) * CurrentShakeOffsetAmplitude.X * NormalizedStrength,
			FMath::Sin(CurrentShakeElapsedTime * Frequency * 1.79f) * CurrentShakeOffsetAmplitude.Y * NormalizedStrength,
			FMath::Cos(CurrentShakeElapsedTime * Frequency * 1.57f) * CurrentShakeOffsetAmplitude.Z * NormalizedStrength);

		SpringArmComponent->SocketOffset = CachedBaseSocketOffset + Oscillation;
		return;
	}

	SpringArmComponent->SocketOffset = FMath::VInterpTo(
		SpringArmComponent->SocketOffset,
		CachedBaseSocketOffset,
		DeltaTime,
		CameraShakeReturnSpeed);
}

void UPlayerCameraRigComponent::TriggerCraftDestroyedShake() const
{
	TriggerProceduralShake(CraftDestroyedShakeOffsetAmplitude, CraftDestroyedShakeDuration);
}

void UPlayerCameraRigComponent::TriggerEnemyDestroyedShake() const
{
	TriggerProceduralShake(EnemyDestroyedShakeOffsetAmplitude, EnemyDestroyedShakeDuration);
}

void UPlayerCameraRigComponent::TriggerProceduralShake(const FVector& InAmplitude, float InDuration) const
{
	if (InDuration <= 0.0f)
	{
		return;
	}

	const float CurrentStrength = CurrentShakeOffsetAmplitude.SizeSquared() * CurrentShakeRemainingTime;
	const float RequestedStrength = InAmplitude.SizeSquared() * InDuration;
	if (RequestedStrength >= CurrentStrength)
	{
		CurrentShakeOffsetAmplitude = InAmplitude;
		CurrentShakeDuration = InDuration;
		CurrentShakeRemainingTime = InDuration;
		CurrentShakeElapsedTime = 0.0f;
	}
}
