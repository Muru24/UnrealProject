#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemyManager.generated.h"

UCLASS()
class UNREALPROJECT_API UEnemyManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UEnemyManager();

	void AddEnemy(APawn* Enemy);
	const TArray<APawn*>& GetEnemys() const { return EnemyArr; }

protected:
	UFUNCTION()
	void HandleTargetDestroyed(AActor* DestroyedActor);

	UPROPERTY()
	TArray<TObjectPtr<APawn>> EnemyArr;
};
