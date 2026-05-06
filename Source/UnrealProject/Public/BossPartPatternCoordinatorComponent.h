#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossPartPatternCoordinatorComponent.generated.h"

class AActor;
class ABossOutPart;
class UBossOutPartPatternComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UBossPartPatternCoordinatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossPartPatternCoordinatorComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	bool TryExecuteCommonPattern(ABossOutPart* TargetPart, AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	bool TryExecuteSpecialPattern(ABossOutPart* TargetPart, AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void StopCurrentPattern();

	UFUNCTION(BlueprintPure, Category = "Boss|Pattern")
	ABossOutPart* GetActivePatternPart() const { return ActivePatternPart; }

private:
	UPROPERTY()
	TObjectPtr<ABossOutPart> ActivePatternPart;

	UPROPERTY()
	TObjectPtr<UBossOutPartPatternComponent> ActivePatternComponent;

	bool CanExecutePattern(ABossOutPart* TargetPart) const;
	void SetActivePattern(ABossOutPart* TargetPart, UBossOutPartPatternComponent* PatternComponent);

	void HandlePatternFinished(UBossOutPartPatternComponent* FinishedPatternComponent);
};
