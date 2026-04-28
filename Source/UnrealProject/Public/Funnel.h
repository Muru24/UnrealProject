// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealProject/Pawn_Template.h"
#include "Funnel.generated.h"

class ABeamEffectActor;
class AActor;
class USceneComponent;

UCLASS()
class UNREALPROJECT_API AFunnel : public APawn_Template
{
	GENERATED_BODY()

public:
	AFunnel();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam|Components")
	TObjectPtr<USceneComponent> FireOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Attack")	
	TSubclassOf<ABeamEffectActor> BeamActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Attack", meta = (ClampMin = "0.01"))
	float AttackActiveDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Attack", meta = (ClampMin = "0.01"))
	float BeamFireInterval = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Attack", meta = (ClampMin = "0.01"))
	float BeamDuration = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Attack")
	bool bAutoStartBeamAttackOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Follow", meta = (ClampMin = "0.0"))
	float FollowSpeed = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Follow", meta = (ClampMin = "0.0"))
	float DesiredDistanceToTarget = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Follow")
	FVector TargetOffset = FVector::ZeroVector;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Beam|Follow")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Follow", meta = (ClampMin = "0.0"))
	float OrbitRadius = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Follow", meta = (ClampMin = "0.0"))
	float OrbitHeight = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Follow", meta = (ClampMin = "0.0"))
	float OrbitAngularSpeed = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Follow|Random")
	bool bRandomizeOrbitPerInstance = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Follow|Random", meta = (ClampMin = "0.0"))
	float OrbitRadiusRandomOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Follow|Random", meta = (ClampMin = "0.0"))
	float OrbitHeightRandomOffset = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Follow|Random", meta = (ClampMin = "0.0"))
	float OrbitSpeedRandomOffset = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Follow|Random", meta = (ClampMin = "0.0"))
	float DistanceRandomOffset = 40.0f;

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

	FTimerHandle BeamFireTimerHandle;
	FTimerHandle BeamDeactivateTimerHandle;
	FTimerHandle AttackStopTimerHandle;
	float OrbitAngleDegrees = 0.0f;
	float RuntimeOrbitRadius = 0.0f;
	float RuntimeOrbitHeight = 0.0f;
	float RuntimeOrbitAngularSpeed = 0.0f;
	float RuntimeDesiredDistanceToTarget = 0.0f;

	void UpdateFollowMovement(float DeltaTime);
	void FireNextBeam();
	void DeactivateActiveBeam();
	FVector GetDesiredBeamTargetLocation() const;
	FVector GetFireOriginLocation() const;
};
