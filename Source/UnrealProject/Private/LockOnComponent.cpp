#include "LockOnComponent.h"

#include "EnemyManager.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	EnemyManager = GetWorld() ? GetWorld()->GetSubsystem<UEnemyManager>() : nullptr;
}

const TArray<APawn*>* ULockOnComponent::GetEnemyList() const
{
	return EnemyManager ? &EnemyManager->GetEnemys() : nullptr;
}

void ULockOnComponent::ClearTarget()
{
	CurrentTarget = nullptr;
	CurrentTargetIndex = INDEX_NONE;
}

void ULockOnComponent::SetCurrentTarget(APawn* NewTarget, int32 NewIndex)
{
	CurrentTarget = NewTarget;
	CurrentTargetIndex = NewTarget ? NewIndex : INDEX_NONE;
	UE_LOG(LogTemp, Display, TEXT("%s"), *CurrentTarget.GetName());
}

int32 ULockOnComponent::FindClosestTargetIndex(const TArray<APawn*>& EnemyList) const
{
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return INDEX_NONE;
	}

	const FVector StartPosition = OwnerActor->GetActorLocation();
	float BestDistanceSq = TNumericLimits<float>::Max();
	int32 BestIndex = INDEX_NONE;

	for (int32 Index = 0; Index < EnemyList.Num(); ++Index)
	{
		APawn* Enemy = EnemyList[Index];
		if (!IsValidLockOnTarget(Enemy))
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(StartPosition, Enemy->GetActorLocation());
		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestIndex = Index;
		}
	}

	return BestIndex;
}

int32 ULockOnComponent::FindNextValidTargetIndex(const TArray<APawn*>& EnemyList, int32 StartIndex) const
{
	if (EnemyList.IsEmpty())
	{
		return INDEX_NONE;
	}

	const int32 SafeStartIndex = StartIndex >= 0 ? StartIndex : 0;
	for (int32 Offset = 0; Offset < EnemyList.Num(); ++Offset)
	{
		const int32 CandidateIndex = (SafeStartIndex + Offset) % EnemyList.Num();
		if (IsValidLockOnTarget(EnemyList[CandidateIndex]))
		{
			return CandidateIndex;
		}
	}

	return INDEX_NONE;
}

bool ULockOnComponent::IsValidLockOnTarget(const APawn* CandidateTarget) const
{
	return IsValid(CandidateTarget) && CandidateTarget != GetOwner();
}

void ULockOnComponent::RefreshCurrentTarget()
{
	if (!bIsLockOnEnabled)
	{
		ClearTarget();
		return;
	}

	const TArray<APawn*>* EnemyList = GetEnemyList();
	if (!EnemyList || EnemyList->IsEmpty())
	{
		bIsLockOnEnabled = false;
		ClearTarget();
		return;
	}

	if (CurrentTargetIndex >= 0 && CurrentTargetIndex < EnemyList->Num())
	{
		APawn* IndexedTarget = (*EnemyList)[CurrentTargetIndex];
		if (IsValidLockOnTarget(IndexedTarget))
		{
			CurrentTarget = IndexedTarget;
			return;
		}
	}

	const int32 ReplacementIndex = FindClosestTargetIndex(*EnemyList);
	if (ReplacementIndex == INDEX_NONE)
	{
		bIsLockOnEnabled = false;
		ClearTarget();
		return;
	}

	SetCurrentTarget((*EnemyList)[ReplacementIndex], ReplacementIndex);
}

void ULockOnComponent::TraceTarget()
{
	bIsLockOnEnabled = !bIsLockOnEnabled;

	if (!bIsLockOnEnabled)
	{
		ClearTarget();
		return;
	}

	const TArray<APawn*>* EnemyList = GetEnemyList();
	if (!EnemyList || EnemyList->IsEmpty())
	{
		bIsLockOnEnabled = false;
		ClearTarget();
		return;
	}

	const int32 BestIndex = FindClosestTargetIndex(*EnemyList);
	if (BestIndex == INDEX_NONE)
	{
		bIsLockOnEnabled = false;
		ClearTarget();
		return;
	}

	SetCurrentTarget((*EnemyList)[BestIndex], BestIndex);
}

void ULockOnComponent::ChangeTarget()
{
	if (!bIsLockOnEnabled)
	{
		return;
	}

	const TArray<APawn*>* EnemyList = GetEnemyList();
	if (!EnemyList || EnemyList->Num() < 2)
	{
		return;
	}

	const int32 NextIndex = FindNextValidTargetIndex(*EnemyList, CurrentTargetIndex + 1);
	if (NextIndex == INDEX_NONE)
	{
		return;
	}

	SetCurrentTarget((*EnemyList)[NextIndex], NextIndex);
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RefreshCurrentTarget();
}
