#pragma once

#include "CoreMinimal.h"
#include "Pawn_Template.h"
#include "SquadRuntimeComponent.h"
#include "P_Player.generated.h"

class ASquadCraftActor;
class UCameraComponent;
class ULockOnComponent;
class UPathFollowerComponent;
class UPlayerAimFireComponent;
class UPlayerCameraRigComponent;
class UPlayerRailMovementComponent;
class URailOffsetComponent;
class USpringArmComponent;
class USquadComponent;
class USupportFireComponent;
class UWidgetComponent;

UCLASS()
class UNREALPROJECT_API AP_Player : public APawn_Template
{
	GENERATED_BODY()

protected:
	AP_Player();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<ULockOnComponent> LockOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPathFollowerComponent> PathFollower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URailOffsetComponent> RailOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USquadComponent> SquadComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USquadRuntimeComponent> SquadRuntimeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPlayerAimFireComponent> PlayerAimFireComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPlayerCameraRigComponent> PlayerCameraRigComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPlayerRailMovementComponent> PlayerRailMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USupportFireComponent> SupportFireComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI|Floating")
	TObjectPtr<UWidgetComponent> LeftFloatingUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Floating")
	FVector LeftUIOffset = FVector(300.0f, -150.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Floating")
	FRotator LeftUIRotation = FRotator::ZeroRotator;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	void Accelerator();
	void StartTestMiniGame();

	virtual void Fire() override;
	virtual void Tick(float DeltaTime) override;
	void MoveHorizontal(float Value);
	void MoveVertical(float Value);
	void SwapSquadLeft();
	void SwapSquadRight();
	void TriggerOffensiveSkill();
	void TriggerBuffSkill();
	void UpdateAutoBuffSkill(float DeltaTime);
	void HandleActiveAutoFire();
	void HandleSupportAutoFire();
	void HandleSquadCraftDefeated(ASquadCraftActor* DefeatedCraft);
	ASquadCraftActor* GetActiveCraft() const;

public:
    UFUNCTION(BlueprintCallable, Category = "Skill")
    const TArray<AActor*>& GetSkillTargetEnemies() const { return SkillTargetEnemies; }

    void SetSkillTargetEnemies(const TArray<AActor*>& InTargets) { SkillTargetEnemies = InTargets; }
    void ClearSkillTargetEnemies() { SkillTargetEnemies.Empty(); }

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Skill")
    TArray<AActor*> SkillTargetEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|AutoBuff", meta = (ClampMin = "0.0"))
	float BuffAutoActivateInterval = 0.25f;

	float BuffAutoActivateTimer = 0.0f;

};
