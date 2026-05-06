#include "BossPartPatternCoordinatorComponent.h"

#include "BossOutPart.h"
#include "BossOutPartPatternComponent.h"

UBossPartPatternCoordinatorComponent::UBossPartPatternCoordinatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBossPartPatternCoordinatorComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UBossPartPatternCoordinatorComponent::TryExecuteCommonPattern(ABossOutPart* TargetPart, AActor* TargetActor)
{
	if (!TargetPart)
	{
		return false;
	}

	UBossOutPartPatternComponent* PatternComponent = TargetPart ? TargetPart->GetPatternComponent() : nullptr;
	if (!PatternComponent)
	{
		return false;
	}

	if (!PatternComponent->FireCommonPattern(TargetActor))
	{
		return false;
	}

	return true;
}

bool UBossPartPatternCoordinatorComponent::TryExecuteSpecialPattern(ABossOutPart* TargetPart, AActor* TargetActor)
{
	if (!CanExecutePattern(TargetPart))
	{
		return false;
	}

	UBossOutPartPatternComponent* PatternComponent = TargetPart ? TargetPart->GetPatternComponent() : nullptr;
	if (!PatternComponent)
	{
		return false;
	}

	if (!PatternComponent->ExecuteSpecialPattern(TargetActor))
	{
		return false;
	}

	SetActivePattern(TargetPart, PatternComponent);
	return true;
}

void UBossPartPatternCoordinatorComponent::StopCurrentPattern()
{
	if (ActivePatternComponent)
	{
		ActivePatternComponent->StopActivePattern();
	}

	ActivePatternPart = nullptr;
	ActivePatternComponent = nullptr;
}

bool UBossPartPatternCoordinatorComponent::CanExecutePattern(ABossOutPart* TargetPart) const
{
	return TargetPart && (!ActivePatternComponent || ActivePatternPart == TargetPart);
}

void UBossPartPatternCoordinatorComponent::SetActivePattern(ABossOutPart* TargetPart, UBossOutPartPatternComponent* PatternComponent)
{
	if (!PatternComponent)
	{
		return;
	}

	if (ActivePatternComponent != PatternComponent)
	{
		PatternComponent->OnPatternFinished.RemoveAll(this);
		PatternComponent->OnPatternFinished.AddUObject(this, &UBossPartPatternCoordinatorComponent::HandlePatternFinished);
	}

	ActivePatternPart = TargetPart;
	ActivePatternComponent = PatternComponent;

	if (!PatternComponent->IsPatternActive())
	{
		HandlePatternFinished(PatternComponent);
	}
}

void UBossPartPatternCoordinatorComponent::HandlePatternFinished(UBossOutPartPatternComponent* FinishedPatternComponent)
{
	if (ActivePatternComponent != FinishedPatternComponent)
	{
		return;
	}

	ActivePatternComponent->OnPatternFinished.RemoveAll(this);
	ActivePatternPart = nullptr;
	ActivePatternComponent = nullptr;
}
