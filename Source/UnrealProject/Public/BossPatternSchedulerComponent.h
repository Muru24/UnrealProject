#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossPatternSchedulerComponent.generated.h"

class APawn_CompositeMaster;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UBossPatternSchedulerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossPatternSchedulerComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Boss|Pattern")
	void SetAutoPatternEnabled(bool bEnabled) { bAutoPatternEnabled = bEnabled; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern")
	bool bAutoPatternEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern", meta = (ClampMin = "0.1"))
	float SpecialPatternInterval = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Pattern", meta = (ClampMin = "0.05"))
	float CommonPatternTickInterval = 0.25f;

private:
	TObjectPtr<APawn_CompositeMaster> OwnerCompositeMaster;
	float SpecialPatternTimer = 0.0f;
	float CommonPatternTimer = 0.0f;

	void TryRunNextSpecialPattern();
	void TryRunCommonPatterns();
	AActor* ResolveTargetActor() const;
};
