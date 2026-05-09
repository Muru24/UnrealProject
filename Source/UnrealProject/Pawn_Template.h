// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnStruct.h"
#include "Pawn_Template.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UPathFollowerComponent;
class UMaterialInstanceDynamic;

UCLASS()
class UNREALPROJECT_API APawn_Template : public APawn
{
	GENERATED_BODY()

public:
	APawn_Template();

protected:
	virtual void BeginPlay() override;

	//기본 유닛 필수 컴포넌트
	// 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	//외형 프리팹
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PlaneMesh; 

	//움직이는 유닛 스텟
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStatComponent* StatComponent;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	float DissolveSpeed = 2.0f;

	float CurrentDissolvePower = 1.0f;
	bool bIsDissolvingIn = false;
	bool bIsDissolvingOut = false;
	bool bHasDissolveParameter = false;
	bool bDissolveInFinished = false;

	virtual void OnDissolveInFinished();
	virtual void OnDissolveOutFinished();

public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintPure, Category = "Effect")
	bool IsDissolveInComplete() const { return bDissolveInFinished || !bHasDissolveParameter; }

	UFUNCTION(BlueprintCallable, Category = "Effect")
	void StartDissolveOut();

	virtual void Fire() {};
};
