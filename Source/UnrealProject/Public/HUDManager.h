// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDManager.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPROJECT_API AHUDManager : public AHUD
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> HUDWidgetClass;

    UPROPERTY()
    class UUserWidget* CurrentHUD;

protected:
    virtual void BeginPlay() override;
};
