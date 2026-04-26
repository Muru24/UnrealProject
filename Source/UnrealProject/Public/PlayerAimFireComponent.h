#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAimFireComponent.generated.h"

class APawn;
class APlayerController;
class ASquadCraftActor;
class ULockOnComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UPlayerAimFireComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerAimFireComponent();

	bool FireActiveCraft(APlayerController* PlayerController, ULockOnComponent* LockOnComponent, ASquadCraftActor* ActiveCraft, APawn* InstigatorPawn) const;

private:
	bool ResolveAimTarget(APlayerController* PlayerController, ULockOnComponent* LockOnComponent, FVector& OutTargetPoint, AActor*& OutTargetActor) const;
};
