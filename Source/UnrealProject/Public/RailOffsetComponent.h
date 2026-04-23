#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RailOffsetComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API URailOffsetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URailOffsetComponent();

	void SetHorizontalInput(float Value);
	void SetVerticalInput(float Value);
	void UpdateOffset(float DeltaTime);

	FVector2D GetCurrentOffset() const { return CurrentOffset; }
	FRotator GetVisualTiltRotation() const { return VisualTiltRotation; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float MaxHorizontalOffset = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float MaxVerticalOffset = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float HorizontalMoveSpeed = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float VerticalMoveSpeed = 550.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float ReturnInterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset|Tilt")
	float MaxPitchTilt = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset|Tilt")
	float MaxRollTilt = 22.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset|Tilt")
	float TiltResponseSpeed = 7.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	FVector2D CurrentOffset = FVector2D::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	FVector2D CurrentInput = FVector2D::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail Offset|Tilt")
	FVector2D OffsetVelocity = FVector2D::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail Offset|Tilt")
	FRotator VisualTiltRotation = FRotator::ZeroRotator;
};
