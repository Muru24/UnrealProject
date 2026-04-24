#pragma once

#include "CoreMinimal.h"
#include "Pawn_Template.h"
#include "SquadComponent.h"
#include "P_Player.generated.h"

UCLASS()
class UNREALPROJECT_API AP_Player : public APawn_Template
{
	GENERATED_BODY()

protected:
	AP_Player();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class ULockOnComponent* LockOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPathFollowerComponent* PathFollower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class URailOffsetComponent* RailOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USquadComponent* SquadComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	TSubclassOf<class ASquadCraftActor> LeftCraftClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	TSubclassOf<class ASquadCraftActor> CenterCraftClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	TSubclassOf<class ASquadCraftActor> RightCraftClass;

	UPROPERTY()
	class ASquadCraftActor* LeftCraft;

	UPROPERTY()
	class ASquadCraftActor* CenterCraft;

	UPROPERTY()
	class ASquadCraftActor* RightCraft;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxCameraOffset = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraMoveSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MouseDeadZone = 0.15f;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	void Accelerator();

	virtual void Fire() override;
	virtual void Tick(float DeltaTime) override;
	void MoveHorizontal(float Value);
	void MoveVertical(float Value);
	void SwapSquadLeft();
	void SwapSquadRight();
	void ApplyRailMovement(float DeltaTime);
	void UpdateCameraPan(float DeltaTime);
	void HandleSupportAutoFire();

	void SpawnSquadCrafts();
	void RefreshSquadCrafts();
	class ASquadCraftActor* GetCraftForSlot(ESquadSlot Slot) const;
	class ASquadCraftActor* GetActiveCraft() const;
	AActor* GetPreferredAutoFireTarget() const;
};
