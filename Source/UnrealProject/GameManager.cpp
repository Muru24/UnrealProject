// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

AGameManager::AGameManager()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("C:/Users/ldjin/Desktop/UnrealProject/Content/01_BluePrint/BP_Player.uasset"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGameManager::AddScore(int Amount)
{
	CurrentScore += Amount;
}

void AGameManager::Initialize_GameManager()
{
	CurrentScore = 0;
	ElapsedTime = 0.0f;
	bIsGameActive = true;
}

