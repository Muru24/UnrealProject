#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SquadComponent.h"
#include "SquadRuntimeComponent.generated.h"

class ASquadCraftActor;
class UStaticMeshComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API USquadRuntimeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USquadRuntimeComponent();

	void SpawnCrafts(AActor* OwnerActor, USquadComponent* SquadLogic, UStaticMeshComponent* FallbackMesh);
	void RefreshCraftStates(const USquadComponent* SquadLogic);
	void ApplyActiveCraftVisualRotation(const USquadComponent* SquadLogic, const FRotator& ActiveCraftRotation);
	ASquadCraftActor* GetCraftForSlot(ESquadSlot Slot) const;
	ASquadCraftActor* GetActiveCraft(const USquadComponent* SquadLogic) const;
	void GetAllCrafts(TArray<ASquadCraftActor*>& OutCrafts) const;
	bool SelectFirstOperationalCraft(USquadComponent* SquadLogic) const;
	bool HasOperationalCrafts() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	TSubclassOf<ASquadCraftActor> LeftCraftClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	TSubclassOf<ASquadCraftActor> CenterCraftClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	TSubclassOf<ASquadCraftActor> RightCraftClass;

	UPROPERTY()
	TObjectPtr<ASquadCraftActor> LeftCraft;

	UPROPERTY()
	TObjectPtr<ASquadCraftActor> CenterCraft;

	UPROPERTY()
	TObjectPtr<ASquadCraftActor> RightCraft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad|Visual")
	float SupportPitchTiltScale = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad|Visual")
	float SupportRollTiltScale = 0.75f;

private:
	void InitializeCraftVisual(ASquadCraftActor* Craft, UStaticMeshComponent* FallbackMesh) const;
	bool IsCraftOperational(const ASquadCraftActor* Craft) const;
	ESquadSlot ResolveDisplaySlotForCraft(ESquadSlot CraftSlot, const USquadComponent* SquadLogic) const;
};
