// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealProject/Pawn_Template.h"
#include "LaserCannon.generated.h"

class ABeamEffectActor;
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
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam|Components")
	TObjectPtr<USceneComponent> FireOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Attack")
	TSubclassOf<ABeamEffectActor> BeamActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Attack", meta = (ClampMin = "0.01"))
	float AttackActiveDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Attack", meta = (ClampMin = "0.01"))
	float BeamDuration = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Attack")
	bool bAutoStartBeamAttackOnBeginPlay = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Beam|Target")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Visual")
	float InitialBeamSize = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Visual")
	float FinalBeamSize = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Target")
	float BeamExtraDistance = 500.0f;

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void StartBeamAttack(AActor* InTarget);

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void StopBeamAttack();

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void SetBeamTarget(AActor* InTarget);

	UFUNCTION(BlueprintPure, Category = "Beam")
	AActor* GetBeamTarget() const { return TargetActor; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Beam|Runtime")
	TObjectPtr<ABeamEffectActor> ActiveBeamActor;

	UPROPERTY(BlueprintReadOnly, Category = "Beam|Runtime")
	bool bAttackActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Beam|Runtime")
	float AttackTimeTracker = 0.0f;

	FTimerHandle BeamDeactivateTimerHandle;
	FTimerHandle AttackStopTimerHandle;

	void FireBeam();
	void DeactivateActiveBeam();
	FVector GetTargetLocationWithExtension() const;
	FVector GetFireOriginLocation() const;
};