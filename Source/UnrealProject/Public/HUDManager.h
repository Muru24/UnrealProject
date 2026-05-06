// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDManager.generated.h"

/**
 * 
 */
class UTargetButtonWidget;

UCLASS()
class UNREALPROJECT_API AHUDManager : public AHUD
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> HUDWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UTargetButtonWidget> TargetWidgetClass;

    UPROPERTY()
    class UUserWidget* CurrentHUD;

    /** 타겟 클릭 미니게임을 시작합니다. */
    UFUNCTION(BlueprintCallable, Category = "UI|MiniGame")
    void StartTargetMiniGame(int32 TotalTargets);

protected:
    UPROPERTY(BlueprintReadOnly, Category = "UI|MiniGame")
    int32 ActiveTargetsCount = 0;

    UFUNCTION()
    void HandleTargetClicked();

protected:
    virtual void BeginPlay() override;
};
