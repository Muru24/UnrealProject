#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDManager.generated.h"

class UTargetButtonWidget;
class UUserWidget;
class UBossOutPartPatternComponent;

UCLASS()
class UNREALPROJECT_API AHUDManager : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> MiniGameBackgroundWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UTargetButtonWidget> TargetWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentHUD;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentMiniGameBackground;

	UFUNCTION(BlueprintCallable, Category = "UI|MiniGame")
	void StartTargetMiniGame(int32 TotalTargets, UBossOutPartPatternComponent* SourcePatternComponent = nullptr);

	UFUNCTION(BlueprintCallable, Category = "UI|MiniGame")
	void ForceFinishMiniGame(bool bTreatAsSuccess = true);

	UFUNCTION(BlueprintPure, Category = "UI|MiniGame")
	bool IsMiniGameActive() const { return bMiniGameActive; }

	UFUNCTION(BlueprintPure, Category = "UI|MiniGame")
	bool IsMiniGameOwnedByPattern(const UBossOutPartPatternComponent* SourcePatternComponent) const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI|MiniGame")
	int32 ActiveTargetsCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|MiniGame")
	float MiniGameTimeLimit = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|MiniGame")
	float MiniGameFailureDamage = 50.0f;

	UFUNCTION()
	void HandleTargetClicked();

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<TObjectPtr<UTargetButtonWidget>> ActiveTargetWidgets;

	UPROPERTY()
	TObjectPtr<UBossOutPartPatternComponent> ActiveMiniGameSourcePatternComponent;

	FTimerHandle MiniGameFailTimerHandle;
	bool bMiniGameActive = false;

	void EndTargetMiniGame(bool bWasSuccessful);
	void HandleMiniGameFailed();
	void ApplyMiniGameFailureDamage();
	void SetMiniGameBattlePaused(bool bPaused);
	void DestroyActiveBullets();
};
