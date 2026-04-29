// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealProject/Pawn_Template.h"
#include "LaserCannon.generated.h"

class ABeamEffectActor;
class ULaserAttackComponent;
class AActor;
class USceneComponent;

/**
 * ALaserCannon
 * 고정된 위치에서 타겟을 조준하며, 시간이 지날수록 레이저가 점점 굵어지는 공격을 수행합니다.
 */
UCLASS()
class UNREALPROJECT_API ALaserCannon : public APawn_Template
{
	GENERATED_BODY()

public:
	ALaserCannon();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	ULaserAttackComponent* LaserComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam|Components")
	TObjectPtr<USceneComponent> FireOrigin;

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void StartBeamAttack(AActor* InTarget);

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void StopBeamAttack();

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void SetBeamTarget(AActor* InTarget);

	UFUNCTION(BlueprintPure, Category = "Beam")
	AActor* GetBeamTarget() const;

protected:
    // 이전 변수들은 LaserComponent 내부에서 관리됩니다.
};