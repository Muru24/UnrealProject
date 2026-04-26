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

	void UpdateCameraPan(APlayerController* PlayerController, USpringArmComponent* SpringArmComponent, float DeltaTime) const;
	void UpdateCameraAnchor(USpringArmComponent* SpringArmComponent, ASquadCraftActor* ActiveCraft, bool bSnapToTarget) const;
	void UpdateCameraZoom(USpringArmComponent* SpringArmComponent, bool bIsAccelerating, float DeltaTime) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraMoveSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float MouseDeadZone = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraAnchorFollowSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
	float AccelerationZoomOutDistance = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
	float CameraZoomInterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
	float CameraZoomReturnInterpSpeed = 2.5f;

private:
	mutable float CachedBaseArmLength = -1.0f;
};
