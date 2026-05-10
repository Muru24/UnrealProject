// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealProject/Pawn_Template.h"
#include "P_Enemy.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPROJECT_API AP_Enemy : public APawn_Template
{
	GENERATED_BODY()
	
public:
	AP_Enemy();
	virtual void BeginPlay() override;

protected:
	virtual void OnDissolveOutFinished() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UEnemyRushComponent* EnemyRushComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Death", meta = (ClampMin = "0.0"))
	float DestroyDelay = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Death")
	bool bEnemyDefeated = false;

private:
	UFUNCTION()
	void HandleHpChanged(float CurrentHp);

	void HandleEnemyDefeated();
	void PlayDefeatCameraShake() const;

};
