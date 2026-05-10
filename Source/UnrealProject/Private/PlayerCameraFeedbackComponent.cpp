#include "PlayerCameraFeedbackComponent.h"

#include "Camera/CameraShakeBase.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCameraRigComponent.h"

UPlayerCameraFeedbackComponent::UPlayerCameraFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerCameraFeedbackComponent::PlayCraftDestroyedShake(APawn* ViewTargetPawn) const
{
	PlayShakeForPawn(ViewTargetPawn, CraftDestroyedShakeClass, CraftDestroyedShakeScale);
	TriggerProceduralRigShake(true);
}

void UPlayerCameraFeedbackComponent::PlayEnemyDestroyedShake(APawn* ViewTargetPawn) const
{
	PlayShakeForPawn(ViewTargetPawn, EnemyDestroyedShakeClass, EnemyDestroyedShakeScale);
	TriggerProceduralRigShake(false);
}

void UPlayerCameraFeedbackComponent::PlayShakeForPawn(APawn* ViewTargetPawn, TSubclassOf<UCameraShakeBase> ShakeClass, float ShakeScale) const
{
	if (!ShakeClass || ShakeScale <= 0.0f)
	{
		return;
	}

	APlayerController* PlayerController = nullptr;
	if (IsValid(ViewTargetPawn))
	{
		PlayerController = Cast<APlayerController>(ViewTargetPawn->GetController());
	}

	if (!PlayerController)
	{
		PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	}

	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		PlayerController->PlayerCameraManager->StartCameraShake(ShakeClass, ShakeScale);
	}
}

void UPlayerCameraFeedbackComponent::TriggerProceduralRigShake(bool bStrongShake) const
{
	if (UPlayerCameraRigComponent* CameraRig = GetOwner() ? GetOwner()->FindComponentByClass<UPlayerCameraRigComponent>() : nullptr)
	{
		if (bStrongShake)
		{
			CameraRig->TriggerCraftDestroyedShake();
		}
		else
		{
			CameraRig->TriggerEnemyDestroyedShake();
		}
	}
}
