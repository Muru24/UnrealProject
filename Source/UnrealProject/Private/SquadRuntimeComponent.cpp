#include "SquadRuntimeComponent.h"

#include "Components/StaticMeshComponent.h"
#include "SquadCraftActor.h"

USquadRuntimeComponent::USquadRuntimeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USquadRuntimeComponent::SpawnCrafts(AActor* OwnerActor, USquadComponent* SquadLogic, UStaticMeshComponent* FallbackMesh)
{
	if (!OwnerActor || !SquadLogic || !GetWorld())
	{
		return;
	}

	// Build a fixed spawn plan for the three squad slots so the runtime component
	// can own both the class selection and the spawned actor references in one place.
	const TSubclassOf<ASquadCraftActor> DefaultCraftClass = ASquadCraftActor::StaticClass();
	const struct FCraftSpawnInfo
	{
		ESquadSlot Slot;
		TSubclassOf<ASquadCraftActor> CraftClass;
		TObjectPtr<ASquadCraftActor>* TargetPointer;
	}
	SpawnInfos[] =
	{
		{ ESquadSlot::Left, LeftCraftClass ? LeftCraftClass : DefaultCraftClass, &LeftCraft },
		{ ESquadSlot::Center, CenterCraftClass ? CenterCraftClass : DefaultCraftClass, &CenterCraft },
		{ ESquadSlot::Right, RightCraftClass ? RightCraftClass : DefaultCraftClass, &RightCraft }
	};

	for (const FCraftSpawnInfo& SpawnInfo : SpawnInfos)
	{
		// Skip slots that already own a spawned craft. This keeps BeginPlay and
		// future re-entry points idempotent instead of duplicating attached actors.
		if (*SpawnInfo.TargetPointer)
		{
			continue;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerActor;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ASquadCraftActor* NewCraft = GetWorld()->SpawnActor<ASquadCraftActor>(
			SpawnInfo.CraftClass,
			OwnerActor->GetActorLocation(),
			OwnerActor->GetActorRotation(),
			SpawnParams);

		if (!NewCraft)
		{
			continue;
		}

		// The craft stays attached to the player root, but its slot identity and
		// desired relative transform are authored here so later systems only ask
		// the runtime component for the current slot/craft relationship.
		NewCraft->AttachToActor(OwnerActor, FAttachmentTransformRules::KeepWorldTransform);
		NewCraft->SetAssignedSlot(SpawnInfo.Slot);
		NewCraft->SetDesiredRelativeTransform(SquadLogic->GetSlotOffset(SpawnInfo.Slot), FRotator::ZeroRotator);
		InitializeCraftVisual(NewCraft, FallbackMesh);
		*SpawnInfo.TargetPointer = NewCraft;
	}
}

void USquadRuntimeComponent::RefreshCraftStates(const USquadComponent* SquadLogic)
{
	if (!SquadLogic)
	{
		return;
	}

	for (ESquadSlot Slot : SquadLogic->GetAllSlots())
	{
		if (ASquadCraftActor* Craft = GetCraftForSlot(Slot))
		{
			const bool bIsCraftActive = Slot == SquadLogic->GetActiveSlot();
			const FRotator TargetRotation = bIsCraftActive ? Craft->GetCurrentRelativeRotation() : FRotator::ZeroRotator;
			Craft->SetActiveCraft(bIsCraftActive);
			Craft->SetDesiredRelativeTransform(SquadLogic->GetSlotOffset(Slot), TargetRotation);
			Craft->SetVisualTiltRotation(FRotator::ZeroRotator);
		}
	}
}

void USquadRuntimeComponent::ApplyActiveCraftVisualRotation(const USquadComponent* SquadLogic, const FRotator& ActiveCraftRotation)
{
	if (!SquadLogic)
	{
		return;
	}

	if (ASquadCraftActor* ActiveCraft = GetActiveCraft(SquadLogic))
	{
		ActiveCraft->SetDesiredRelativeTransform(
			SquadLogic->GetSlotOffset(SquadLogic->GetActiveSlot()),
			FRotator(ActiveCraftRotation.Pitch, 0.0f, ActiveCraftRotation.Roll));
		ActiveCraft->SetVisualTiltRotation(FRotator::ZeroRotator);
	}

	for (ESquadSlot Slot : SquadLogic->GetAllSlots())
	{
		if (Slot == SquadLogic->GetActiveSlot())
		{
			continue;
		}

		ASquadCraftActor* Craft = GetCraftForSlot(Slot);
		if (!Craft)
		{
			continue;
		}

		Craft->SetVisualTiltRotation(FRotator(
			ActiveCraftRotation.Pitch * SupportPitchTiltScale,
			0.0f,
			ActiveCraftRotation.Roll * SupportRollTiltScale));
	}
}

ASquadCraftActor* USquadRuntimeComponent::GetCraftForSlot(ESquadSlot Slot) const
{
	switch (Slot)
	{
	case ESquadSlot::Left:
		return LeftCraft;
	case ESquadSlot::Right:
		return RightCraft;
	case ESquadSlot::Center:
	default:
		return CenterCraft;
	}
}

ASquadCraftActor* USquadRuntimeComponent::GetActiveCraft(const USquadComponent* SquadLogic) const
{
	return SquadLogic ? GetCraftForSlot(SquadLogic->GetActiveSlot()) : nullptr;
}

void USquadRuntimeComponent::GetAllCrafts(TArray<ASquadCraftActor*>& OutCrafts) const
{
	OutCrafts.Reset();
	OutCrafts.Reserve(3);
	OutCrafts.Add(LeftCraft);
	OutCrafts.Add(CenterCraft);
	OutCrafts.Add(RightCraft);
}

void USquadRuntimeComponent::InitializeCraftVisual(ASquadCraftActor* Craft, UStaticMeshComponent* FallbackMesh) const
{
	if (!Craft || !FallbackMesh || !Craft->GetCraftMesh() || Craft->GetCraftMesh()->GetStaticMesh())
	{
		return;
	}

	// If a squad craft BP does not provide its own mesh, inherit the player's
	// base mesh and materials so placeholder craft actors still render correctly.
	Craft->GetCraftMesh()->SetStaticMesh(FallbackMesh->GetStaticMesh());
	Craft->GetCraftMesh()->SetRelativeScale3D(FallbackMesh->GetRelativeScale3D());

	for (int32 MaterialIndex = 0; MaterialIndex < FallbackMesh->GetNumMaterials(); ++MaterialIndex)
	{
		Craft->GetCraftMesh()->SetMaterial(MaterialIndex, FallbackMesh->GetMaterial(MaterialIndex));
	}
}
