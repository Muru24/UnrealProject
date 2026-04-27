// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealProject/Pawn_Template.h"
#include "Funnel.generated.h"

/**
 * 
 */
class UNiagaraComponent;
UCLASS()
class UNREALPROJECT_API AFunnel : public APawn_Template
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> BeamEffect;

};
