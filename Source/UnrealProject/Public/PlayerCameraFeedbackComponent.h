#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCameraFeedbackComponent.generated.h"

class UCameraShakeBase;
class UPlayerCameraRigComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREALPROJECT_API UPlayerCameraFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerCameraFeedbackComponent();

	UFUNCTION(BlueprintCallable, Category = "Camera|Feedback")
	void PlayCraftDestroyedShake(APawn* ViewTargetPawn) const;

	UFUNCTION(BlueprintCallable, Category = "Camera|Feedback")
	void PlayEnemyDestroyedShake(APawn* ViewTargetPawn) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Feedback")
	TSubclassOf<UCameraShakeBase> CraftDestroyedShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Feedback", meta = (ClampMin = "0.0"))
	float CraftDestroyedShakeScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Feedback")
	TSubclassOf<UCameraShakeBase> EnemyDestroyedShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Feedback", meta = (ClampMin = "0.0"))
	float EnemyDestroyedShakeScale = 0.35f;

private:
	void PlayShakeForPawn(APawn* ViewTargetPawn, TSubclassOf<UCameraShakeBase> ShakeClass, float ShakeScale) const;
	void TriggerProceduralRigShake(bool bStrongShake) const;
};
