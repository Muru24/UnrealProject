// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HomingMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPROJECT_API UHomingMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHomingMovementComponent();

	void SetTurnInterpSpeed(float InTurnInterpSpeed) { TurnInterpSpeed = FMath::Max(0.0f, InTurnInterpSpeed); }
	void SetHomingActivationDelay(float InHomingActivationDelay);

public:	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float TurnInterpSpeed = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float HomingActivationDelay = 0.0f;

	float HomingElapsedTime = 0.0f;
};
