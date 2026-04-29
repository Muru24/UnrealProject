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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USnakeBodyChargeComponent* BodyChargeComponent;
};
