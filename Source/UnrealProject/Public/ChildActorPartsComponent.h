#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChildActorPartsComponent.generated.h"

class UChildActorComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UChildActorPartsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UChildActorPartsComponent();

	void RefreshChildParts();
	void GetChildParts(TArray<UChildActorComponent*>& OutChildParts) const;
	void GetChildPartActors(TArray<AActor*>& OutChildPartActors) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parts")
	bool bRegisterPawnChildrenAsEnemies = true;

private:
	UPROPERTY()
	TArray<TObjectPtr<UChildActorComponent>> CachedChildParts;

	void RegisterPartsToEnemyManager() const;
};
