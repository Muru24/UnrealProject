// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealProject/Pawn_Template.h"
#include "SnakeBody.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPROJECT_API ASnakeBody : public APawn_Template
{
	GENERATED_BODY()
	
public:
	ASnakeBody();

protected:
	virtual void BeginPlay() override;
	virtual void OnDissolveOutFinished() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USnakeBodyChargeComponent* BodyChargeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Snake|Death", meta = (ClampMin = "0.0"))
	float DestroyDelay = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Snake|Death")
	bool bSegmentDefeated = false;

private:
	UFUNCTION()
	void HandleHpChanged(float CurrentHp);

	void HandleSegmentDefeated();
};
