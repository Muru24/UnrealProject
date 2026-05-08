#include "PlayerCameraRigComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "SquadCraftActor.h"

UPlayerCameraRigComponent::UPlayerCameraRigComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerCameraRigComponent::UpdateCameraPan(APlayerController* PlayerController, USpringArmComponent* SpringArmComponent, float DeltaTime) const
{
	if (!PlayerController || !SpringArmComponent)
	{
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
