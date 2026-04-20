// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Pawn_Template.generated.h"

UCLASS()
class UNREALPROJECT_API APawn_Template : public APawn
{
	GENERATED_BODY()

public:
	APawn_Template();

protected:
	virtual void BeginPlay() override;

	//비행기 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;

	//비행기 외형 프리팹
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* PlaneMesh; 

	//카메라 지지대 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArm; 

	//카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* Camera; 

	//최고속도
	UPROPERTY(EditAnywhere, Category = "Flight")
	float MaxSpeed = 5000.f;

	//가속도
	UPROPERTY(EditAnywhere, Category = "Flight")
	float Acceleration = 2000.f;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Initialize_GameManager_Pawn();

private:
	//플레이어 이동 관련
	void Move_Pawn(float DeltaTime);
};
