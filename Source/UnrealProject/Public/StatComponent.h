// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnStruct.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float, CurrentHp);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPROJECT_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FMovePawn Stats;

	void ApplyDamage(float DamageAmount);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHpChangedDelegate OnHpChanged;

	float GetHpPercentage() const { return Stats.unit.HP / Stats.unit.MaxHP; }

	float GetMoveSpeed() const { return Stats.move.MoveSpeed; }
	float GetAcceleration() const { return Stats.move.Acceleration; }
};
