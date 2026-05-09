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
	bool TryAutoFireActiveCraft(APlayerController* PlayerController, ULockOnComponent* LockOnComponent, ASquadCraftActor* ActiveCraft, APawn* InstigatorPawn) const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float PriorityTargetRange = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float MouseAimDistance = 10000.0f;

	bool ResolveAimTarget(APlayerController* PlayerController, ULockOnComponent* LockOnComponent, ASquadCraftActor* ActiveCraft, FVector& OutTargetPoint, AActor*& OutTargetActor) const;
	AActor* GetPriorityEnemyInRange(const FVector& Origin) const;
	AActor* GetNearestEnemy(const FVector& Origin) const;
	AActor* GetRandomEnemy() const;
};
