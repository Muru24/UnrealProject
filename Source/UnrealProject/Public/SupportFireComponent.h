#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftLoadoutComponent.h"
#include "SupportFireComponent.generated.h"

class APawn;
class ASquadCraftActor;
class ULockOnComponent;
class USquadComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API USupportFireComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USupportFireComponent();

	void HandleSupportAutoFire(const TArray<ASquadCraftActor*>& SquadCrafts, ULockOnComponent* LockOnComponent, APawn* InstigatorPawn) const;

private:
	AActor* GetSupportAutoFireTargetForCraft(const ASquadCraftActor* Craft, ULockOnComponent* LockOnComponent) const;
	AActor* GetNearestEnemy(const FVector& Origin) const;
	bool ShouldSupportCraftAutoFire(const ASquadCraftActor* Craft) const;
};
