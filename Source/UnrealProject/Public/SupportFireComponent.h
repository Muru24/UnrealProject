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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float PriorityTargetRange = 1200.0f;

	AActor* GetSupportAutoFireTargetForCraft(const ASquadCraftActor* Craft, ULockOnComponent* LockOnComponent) const;
	AActor* GetPriorityEnemyInRange(const FVector& Origin) const;
	AActor* GetNearestEnemy(const FVector& Origin) const;
	AActor* GetRandomEnemy() const;
	bool ShouldSupportCraftAutoFire(const ASquadCraftActor* Craft) const;
};
