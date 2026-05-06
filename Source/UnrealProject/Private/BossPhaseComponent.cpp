#include "BossPhaseComponent.h"

#include "StatComponent.h"

UBossPhaseComponent::UBossPhaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBossPhaseComponent::InitializePhaseTracking(UStatComponent* InStatComponent)
{
	if (TrackedStatComponent == InStatComponent)
	{
		return;
	}

	if (TrackedStatComponent)
	{
		TrackedStatComponent->OnHpChanged.RemoveDynamic(this, &UBossPhaseComponent::HandleHpChanged);
	}

	TrackedStatComponent = InStatComponent;

	if (TrackedStatComponent)
	{
		TrackedStatComponent->OnHpChanged.AddDynamic(this, &UBossPhaseComponent::HandleHpChanged);
	}

	EvaluatePhase();
}

void UBossPhaseComponent::StartEncounter()
{
	bEncounterStarted = true;
	EvaluatePhase();
}

void UBossPhaseComponent::HandleHpChanged(float CurrentHp)
{
	EvaluatePhase();
}

void UBossPhaseComponent::EvaluatePhase()
{
	if (!TrackedStatComponent)
	{
		return;
	}

	const float HpPercent = TrackedStatComponent->GetHpPercentage();

	if (HpPercent <= 0.0f)
	{
		SetPhase(EBossEncounterPhase::Defeated);
		return;
	}

	if (!bEncounterStarted)
	{
		SetPhase(EBossEncounterPhase::Intro);
		return;
	}

	if (HpPercent <= EnragedHpThreshold)
	{
		SetPhase(EBossEncounterPhase::Enraged);
		return;
	}

	if (HpPercent <= Phase2HpThreshold)
	{
		SetPhase(EBossEncounterPhase::Phase2);
		return;
	}

	SetPhase(EBossEncounterPhase::Phase1);
}

void UBossPhaseComponent::SetPhase(EBossEncounterPhase NewPhase)
{
	if (CurrentPhase == NewPhase)
	{
		return;
	}

	const EBossEncounterPhase PreviousPhase = CurrentPhase;
	CurrentPhase = NewPhase;
	OnBossPhaseChanged.Broadcast(PreviousPhase, CurrentPhase);
}
