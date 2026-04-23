#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SquadComponent.generated.h"

UENUM(BlueprintType)
enum class ESquadSlot : uint8
{
	Left,
	Center,
	Right
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActiveSquadSlotChanged, ESquadSlot, PreviousSlot, ESquadSlot, NewSlot);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API USquadComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USquadComponent();

	void ShiftActiveSlotLeft();
	void ShiftActiveSlotRight();

	ESquadSlot GetActiveSlot() const { return ActiveSlot; }
	FVector GetSlotOffset(ESquadSlot Slot) const;
	TArray<ESquadSlot> GetAllSlots() const;

	UPROPERTY(BlueprintAssignable, Category = "Squad")
	FOnActiveSquadSlotChanged OnActiveSlotChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	float FormationSpacing = 260.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	float SupportDepthOffset = -40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	float SupportVerticalOffset = -45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	ESquadSlot ActiveSlot = ESquadSlot::Center;

	void SetActiveSlot(ESquadSlot NewSlot);
};
