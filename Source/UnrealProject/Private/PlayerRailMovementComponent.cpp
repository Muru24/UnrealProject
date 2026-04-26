#include "PlayerRailMovementComponent.h"

#include "..\PathFollowerComponent.h"
#include "RailOffsetComponent.h"
#include "SquadComponent.h"
#include "SquadRuntimeComponent.h"

UPlayerRailMovementComponent::UPlayerRailMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerRailMovementComponent::ApplyRailMovement(
	AActor* OwnerActor,
	UPathFollowerComponent* PathFollowerComponent,
	URailOffsetComponent* RailOffsetComponent,
	USquadComponent* SquadLogic,
	USquadRuntimeComponent* SquadRuntime,
	float DeltaTime) const
{
	if (!OwnerActor || !PathFollowerComponent || !RailOffsetComponent || !PathFollowerComponent->HasValidBaseTransform())
	{
		return;
	}

	RailOffsetComponent->UpdateOffset(DeltaTime);

	const FTransform& BaseWorldTransform = PathFollowerComponent->GetBaseWorldTransform();
	const FVector2D LocalOffset = RailOffsetComponent->GetCurrentOffset();
	const FRotator VisualTiltRotation = RailOffsetComponent->GetVisualTiltRotation();

	const FVector FinalLocation =
		BaseWorldTransform.GetLocation()
		+ (BaseWorldTransform.GetUnitAxis(EAxis::Y) * LocalOffset.X)
		+ (BaseWorldTransform.GetUnitAxis(EAxis::Z) * LocalOffset.Y);

	const FRotator BaseRotation = BaseWorldTransform.Rotator();
	OwnerActor->SetActorLocationAndRotation(FinalLocation, BaseRotation);

	if (SquadRuntime)
	{
		SquadRuntime->RefreshCraftStates(SquadLogic);
		SquadRuntime->ApplyActiveCraftVisualRotation(SquadLogic, VisualTiltRotation);
	}
}
