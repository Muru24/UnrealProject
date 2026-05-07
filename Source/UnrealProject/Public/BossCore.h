#pragma once

#include "CoreMinimal.h"
#include "UnrealProject/Pawn_Template.h"
#include "BossCore.generated.h"

class ASnake_CompositeMaster;
class APawn_CompositeMaster;

UCLASS()
class UNREALPROJECT_API ABossCore : public APawn_Template
{
	GENERATED_BODY()

public:
	ABossCore();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleHpChanged(float CurrentHp);

public:
	UPROPERTY(EditAnywhere, Category = "Boss|Settings")
	TSubclassOf<ASnake_CompositeMaster> SnakeClass;

	UPROPERTY(BlueprintReadOnly, Category = "Boss|Runtime")
	TObjectPtr<ASnake_CompositeMaster> SpawnedSnake;

	UFUNCTION(BlueprintCallable, Category = "Boss|Death")
	void HandleBossDefeated();

	UFUNCTION(BlueprintCallable, Category = "Boss|Death")
	void RequestBossDeathSequence();

	UFUNCTION(BlueprintCallable, Category = "Boss|Death")
	void RequestGameClear();

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss|Death")
	void ReceiveBossDeathSequenceRequested();

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss|Death")
	void ReceiveGameClearRequested();

	UFUNCTION(BlueprintPure, Category = "Boss|Runtime")
	APawn_CompositeMaster* GetOwningCompositeMaster() const;

private:
	bool bBossDefeatHandled = false;
};
