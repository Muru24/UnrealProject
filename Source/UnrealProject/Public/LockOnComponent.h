#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"

class UEnemyManager;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockOnComponent();

	void TraceTarget();
	void ChangeTarget();

	bool IsLockOnEnabled() const { return bIsLockOnEnabled; }
	APawn* GetCurrentTarget() const { return CurrentTarget.Get(); }
	int32 GetCurrentTargetIndex() const { return CurrentTargetIndex; }

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
	bool bIsLockOnEnabled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
	TObjectPtr<APawn> CurrentTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
	int32 CurrentTargetIndex = INDEX_NONE;

	UPROPERTY()
	TObjectPtr<UEnemyManager> EnemyManager = nullptr;

private:
	const TArray<APawn*>* GetEnemyList() const;
	void ClearTarget();
	void SetCurrentTarget(APawn* NewTarget, int32 NewIndex);
	int32 FindClosestTargetIndex(const TArray<APawn*>& EnemyList) const;
	int32 FindNextValidTargetIndex(const TArray<APawn*>& EnemyList, int32 StartIndex) const;
	bool IsValidLockOnTarget(const APawn* CandidateTarget) const;
	void RefreshCurrentTarget();
};
