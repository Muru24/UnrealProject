#pragma once

#include "CoreMinimal.h"
#include "BossPhaseComponent.h"
#include "UnrealProject/Pawn_Template.h"
#include "BossCore.generated.h"

class ASnake_CompositeMaster;

UCLASS()
class UNREALPROJECT_API ABossCore : public APawn_Template
{
	GENERATED_BODY()

public:
	ABossCore();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleBossPhaseChanged(EBossEncounterPhase PreviousPhase, EBossEncounterPhase NewPhase);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Components")
	TObjectPtr<UBossPhaseComponent> BossPhaseComponent;

	UPROPERTY(EditAnywhere, Category = "Boss|Settings")
	TSubclassOf<ASnake_CompositeMaster> SnakeClass;

	UPROPERTY(BlueprintReadOnly, Category = "Boss|Runtime")
	TObjectPtr<ASnake_CompositeMaster> SpawnedSnake;
};
