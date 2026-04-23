#include "SquadComponent.h"

USquadComponent::USquadComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USquadComponent::ShiftActiveSlotLeft()
{
	switch (ActiveSlot)
	{
	case ESquadSlot::Left:
		SetActiveSlot(ESquadSlot::Right);
		break;
	case ESquadSlot::Center:
		SetActiveSlot(ESquadSlot::Left);
		break;
	case ESquadSlot::Right:
	default:
		SetActiveSlot(ESquadSlot::Center);
		break;
	}
}

void USquadComponent::ShiftActiveSlotRight()
{
	switch (ActiveSlot)
	{
	case ESquadSlot::Left:
		SetActiveSlot(ESquadSlot::Center);
		break;
	case ESquadSlot::Center:
		SetActiveSlot(ESquadSlot::Right);
		break;
	case ESquadSlot::Right:
	default:
		SetActiveSlot(ESquadSlot::Left);
		break;
	}
}

FVector USquadComponent::GetSlotOffset(ESquadSlot Slot) const
{
	switch (Slot)
	{
	case ESquadSlot::Left:
		return FVector(SupportDepthOffset, -FormationSpacing, SupportVerticalOffset);
	case ESquadSlot::Right:
		return FVector(SupportDepthOffset, FormationSpacing, SupportVerticalOffset);
	case ESquadSlot::Center:
	default:
		return FVector::ZeroVector;
	}
}

TArray<ESquadSlot> USquadComponent::GetAllSlots() const
{
	return { ESquadSlot::Left, ESquadSlot::Center, ESquadSlot::Right };
}

void USquadComponent::SetActiveSlot(ESquadSlot NewSlot)
{
	if (ActiveSlot == NewSlot)
	{
		return;
	}

	const ESquadSlot PreviousSlot = ActiveSlot;
	ActiveSlot = NewSlot;
	OnActiveSlotChanged.Broadcast(PreviousSlot, ActiveSlot);
}
