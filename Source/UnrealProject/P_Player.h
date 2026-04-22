// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn_Template.h"
#include "P_Player.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPROJECT_API AP_Player : public APawn_Template
{
	GENERATED_BODY()

protected:
	AP_Player();

	//카메라 지지대 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArm;

	//카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* Camera;

	//록온 기능
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class ULockOnComponent* LockOn;

	//직선 및 유도총알 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBulletWeaponComponent* BWeaponComp;

	//라인 따라가기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPathFollowerComponent* PathFollower;
	
	virtual void BeginPlay() override;
	void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void Fire() override;
};
