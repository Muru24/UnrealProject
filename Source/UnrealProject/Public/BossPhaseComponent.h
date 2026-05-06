#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossPhaseComponent.generated.h"

class UStatComponent;

UENUM(BlueprintType)
enum class EBossEncounterPhase : uint8
{
	Intro,
	Phase1,
	Phase2,
	Enraged,
	Defeated
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossPhaseChanged, EBossEncounterPhase, PreviousPhase, EBossEncounterPhase, NewPhase);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UBossPhaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossPhaseComponent();

	void InitializePhaseTracking(UStatComponent* InStatComponent);
	void StartEncounter();

	EBossEncounterPhase GetCurrentPhase() const { return CurrentPhase; }
	bool IsEncounterStarted() const { return bEncounterStarted; }

	UPROPERTY(BlueprintAssignable, Category = "Boss|Phase")
	FOnBossPhaseChanged OnBossPhaseChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Phase2HpThreshold = 0.66f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EnragedHpThreshold = 0.33f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	EBossEncounterPhase CurrentPhase = EBossEncounterPhase::Intro;

private:
	UPROPERTY()
	TObjectPtr<UStatComponent> TrackedStatComponent;

	bool bEncounterStarted = false;

	UFUNCTION()
	void HandleHpChanged(float CurrentHp);

	void EvaluatePhase();
	void SetPhase(EBossEncounterPhase NewPhase);
};
