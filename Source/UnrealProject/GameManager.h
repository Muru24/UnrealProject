// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameManager.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPROJECT_API AGameManager : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGameManager();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	//스코어 추가 
	UFUNCTION(BlueprintCallable, Category = "GameRules")
	void AddScore(int Amount);

	//현재 점수 반환
	UFUNCTION(BlueprintCallable, Category = "GameRules")
	int GetCurrentScore() const { return CurrentScore; }

	//게임 진행 시간 반환
	UFUNCTION(BlueprintCallable, Category = "GameRules")
	float GetElapsedTime() const { return ElapsedTime; }

	//게임매니저 초기화
	UFUNCTION(BlueprintCallable, Category = "GameRules")
	void Initialize_GameManager();

	//게임모드 변경
	UFUNCTION(BlueprintCallable, Category = "GameRules")
	void ChangeGameMode() { bIsGameActive = !bIsGameActive; }

protected:
	//현재 점수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData")
	int CurrentScore;

	//게임 진행 시간
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData")
	float ElapsedTime;

	//게임 진행여부 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData")
	bool bIsGameActive;
};
