// Fill out your copyright notice in the Description page of Project Settings.
#include "HUDManager.h"
#include "Blueprint/UserWidget.h"

void AHUDManager::BeginPlay()
{
    Super::BeginPlay();

    if (HUDWidgetClass)
    {
        CurrentHUD = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
        if (CurrentHUD)
        {
            CurrentHUD->AddToViewport();
        }
    }
}