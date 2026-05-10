#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pawn_CompositeMaster.generated.h"

class UChildActorComponent;
class UChildActorPartsComponent;
class UBossPartPatternCoordinatorComponent;
class UBossPatternSchedulerComponent;
class UBossSupportPartOrbitComponent;
class ABossCore;
class ABossOutPart;
class USnakeBodyChargeComponent;
class UBossOutPartPatternComponent;

UCLASS()
class UNREALPROJECT_API APawn_CompositeMaster : public AActor
{
	GENERATED_BODY()

public:
	APawn_CompositeMaster();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Composite")
	void GetChildPartActors(TArray<AActor*>& OutChildPartActors) const;

	UFUNCTION(BlueprintCallable, Category = "Composite")
	void GetChildPartComponents(TArray<UChildActorComponent*>& OutChildPartComponents) const;

	UFUNCTION(BlueprintCallable, Category = "Composite")
	void GetSupportPartActors(TArray<AActor*>& OutSupportPartActors) const;

	UFUNCTION(BlueprintPure, Category = "Composite")
	ABossCore* GetBossCore() const;

	UFUNCTION(BlueprintPure, Category = "Composite")
	UBossPartPatternCoordinatorComponent* GetBossPartPatternCoordinatorComponent() const { return BossPartPatternCoordinatorComponent; }

	UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
	void StopAllBossCombat();

	UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
	void RequestCombatRestartSequence();

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss|Combat")
	void ReceiveCombatRestartSequenceRequested();

	UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
	void RestartBossCombat();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UChildActorPartsComponent> ChildActorPartsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBossPartPatternCoordinatorComponent> BossPartPatternCoordinatorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBossPatternSchedulerComponent> BossPatternSchedulerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBossSupportPartOrbitComponent> BossSupportPartOrbitComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<UChildActorComponent*> ChildParts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Combat", meta = (ClampMin = "0.0"))
	float CombatRestartDelay = 1.25f;

	FTimerHandle CombatRestartTimerHandle;
};
