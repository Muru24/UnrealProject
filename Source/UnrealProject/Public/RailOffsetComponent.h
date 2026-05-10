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
	FVector2D GetCurrentInput() const { return CurrentInput; }
	FRotator GetVisualTiltRotation() const { return VisualTiltRotation; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float MaxHorizontalOffset = 380.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float MaxVerticalOffset = 240.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float HorizontalMoveSpeed = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float VerticalMoveSpeed = 550.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float HorizontalInputInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float VerticalInputInterpSpeed = 8.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float HorizontalReturnInterpSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	float VerticalReturnInterpSpeed = 4.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset|Tilt")
	float MaxPitchTilt = 14.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset|Tilt")
	float MaxRollTilt = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rail Offset|Tilt")
	float TiltResponseSpeed = 8.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	FVector2D CurrentOffset = FVector2D::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail Offset")
	FVector2D CurrentInput = FVector2D::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail Offset|Tilt")
	FVector2D OffsetVelocity = FVector2D::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail Offset|Tilt")
	FRotator VisualTiltRotation = FRotator::ZeroRotator;
};
