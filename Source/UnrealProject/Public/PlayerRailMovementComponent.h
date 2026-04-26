#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerRailMovementComponent.generated.h"

class UPathFollowerComponent;
class URailOffsetComponent;
class USquadComponent;
class USquadRuntimeComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UPlayerRailMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerRailMovementComponent();

	void ApplyRailMovement(
		AActor* OwnerActor,
		UPathFollowerComponent* PathFollowerComponent,
		URailOffsetComponent* RailOffsetComponent,
		USquadComponent* SquadLogic,
		USquadRuntimeComponent* SquadRuntime,
		float DeltaTime) const;
};
