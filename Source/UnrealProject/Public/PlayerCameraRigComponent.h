#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCameraRigComponent.generated.h"

class ASquadCraftActor;
class USpringArmComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UPlayerCameraRigComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerCameraRigComponent();

	void UpdateCameraPan(APlayerController* PlayerController, USpringArmComponent* SpringArmComponent, float DeltaTime, bool bSuppressPan = false) const;
	void UpdateCameraAnchor(USpringArmComponent* SpringArmComponent, ASquadCraftActor* ActiveCraft, bool bSnapToTarget) const;
	void UpdateCameraZoom(USpringArmComponent* SpringArmComponent, bool bIsAccelerating, float DeltaTime) const;
	void UpdateCameraShake(USpringArmComponent* SpringArmComponent, float DeltaTime) const;
	void TriggerCraftDestroyedShake() const;
	void TriggerEnemyDestroyedShake() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraMoveSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float MouseDeadZone = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraAnchorFollowSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	FVector CameraAnchorInfluence = FVector(0.0f, 0.35f, 0.25f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
	float AccelerationZoomOutDistance = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
	float CameraZoomInterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
	float CameraZoomReturnInterpSpeed = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Shake", meta = (ClampMin = "0.0"))
	float CraftDestroyedShakeDuration = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Shake")
	FVector CraftDestroyedShakeOffsetAmplitude = FVector(0.0f, 24.0f, 18.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Shake", meta = (ClampMin = "0.0"))
	float EnemyDestroyedShakeDuration = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Shake")
	FVector EnemyDestroyedShakeOffsetAmplitude = FVector(0.0f, 8.0f, 6.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Shake", meta = (ClampMin = "0.0"))
	float CameraShakeFrequency = 32.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Shake", meta = (ClampMin = "0.0"))
	float CameraShakeReturnSpeed = 18.0f;

private:
	mutable float CachedBaseArmLength = -1.0f;
	mutable FVector CachedBaseArmLocation = FVector::ZeroVector;
	mutable bool bHasCachedBaseArmLocation = false;
	mutable FVector CachedBaseSocketOffset = FVector::ZeroVector;
	mutable bool bHasCachedBaseSocketOffset = false;
	mutable float CurrentShakeRemainingTime = 0.0f;
	mutable float CurrentShakeDuration = 0.0f;
	mutable float CurrentShakeElapsedTime = 0.0f;
	mutable FVector CurrentShakeOffsetAmplitude = FVector::ZeroVector;

	void TriggerProceduralShake(const FVector& InAmplitude, float InDuration) const;
};
