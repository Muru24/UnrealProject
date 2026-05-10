#include "BossCore.h"

#include "BossPartPatternCoordinatorComponent.h"
#include "../P_Player.h"
#include "Components/SphereComponent.h"
#include "HUDManager.h"
#include "Kismet/GameplayStatics.h"
#include "Pawn_CompositeMaster.h"
#include "BossOutPart.h"
#include "PlayerCameraFeedbackComponent.h"
#include "Snake_CompositeMaster.h"
#include "StatComponent.h"

ABossCore::ABossCore()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABossCore::BeginPlay()
{
	Super::BeginPlay();

	if (StatComponent)
	{
		StatComponent->OnHpChanged.AddDynamic(this, &ABossCore::HandleHpChanged);
	}

	if (IsDissolveInComplete())
	{
		SpawnSnakeActor();
	}
}

void ABossCore::OnDissolveInFinished()
{
	Super::OnDissolveInFinished();
	SpawnSnakeActor();
}

void ABossCore::SpawnSnakeActor()
{
	if (!SnakeClass)
	{
		return;
	}

	if (SpawnedSnake)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	SpawnedSnake = GetWorld()->SpawnActor<ASnake_CompositeMaster>(SnakeClass, GetActorLocation(), GetActorRotation(), SpawnParams);
	if (!SpawnedSnake)
	{
		return;
	}

	SpawnedSnake->CenterActor = this;
}

void ABossCore::HandleHpChanged(float CurrentHp)
{
	if (CurrentHp > 0.0f || bBossDefeatHandled)
	{
		return;
	}

	HandleBossDefeated();
}

void ABossCore::HandleBossDefeated()
{
	if (bBossDefeatHandled)
	{
		return;
	}

	bBossDefeatHandled = true;

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (AHUDManager* HUDManager = Cast<AHUDManager>(PlayerController->GetHUD()))
		{
			HUDManager->ForceFinishMiniGame(true);
		}
	}

	if (AP_Player* PlayerPawn = Cast<AP_Player>(UGameplayStatics::GetPlayerPawn(this, 0)))
	{
		if (UPlayerCameraFeedbackComponent* CameraFeedback = PlayerPawn->GetPlayerCameraFeedbackComponent())
		{
			CameraFeedback->PlayCraftDestroyedShake(PlayerPawn);
		}
	}

	if (APawn_CompositeMaster* OwningCompositeMaster = GetOwningCompositeMaster())
	{
		OwningCompositeMaster->StopAllBossCombat();
	}

	if (SpawnedSnake)
	{
		SpawnedSnake->StopAllBossCombat();
		SpawnedSnake->SetActorTickEnabled(false);
	}

	SetActorEnableCollision(false);
	SetActorTickEnabled(true);

	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionComponent->SetGenerateOverlapEvents(false);
	}

	if (bHasDissolveParameter && DynamicMaterial)
	{
		bPendingDestroyAfterDeathSequence = true;
		StartDissolveOut();
	}
	else
	{
		RequestBossDeathSequence();
		RequestGameClear();

		if (IsValid(SpawnedSnake))
		{
			SpawnedSnake->Destroy();
			SpawnedSnake = nullptr;
		}

		if (APawn_CompositeMaster* OwningCompositeMaster = GetOwningCompositeMaster())
		{
			OwningCompositeMaster->Destroy();
		}
		else
		{
			Destroy();
		}
	}
}

void ABossCore::OnDissolveOutFinished()
{
	Super::OnDissolveOutFinished();
	RequestBossDeathSequence();
	RequestGameClear();

	if (IsValid(SpawnedSnake))
	{
		SpawnedSnake->Destroy();
		SpawnedSnake = nullptr;
	}

	if (bPendingDestroyAfterDeathSequence)
	{
		if (APawn_CompositeMaster* OwningCompositeMaster = GetOwningCompositeMaster())
		{
			OwningCompositeMaster->Destroy();
		}
		else
		{
			Destroy();
		}
	}
}

void ABossCore::RequestBossDeathSequence()
{
	ReceiveBossDeathSequenceRequested();
}

void ABossCore::RequestGameClear()
{
	ReceiveGameClearRequested();
}

APawn_CompositeMaster* ABossCore::GetOwningCompositeMaster() const
{
	if (const USceneComponent* ParentSceneComponent = GetParentComponent())
	{
		return Cast<APawn_CompositeMaster>(ParentSceneComponent->GetOwner());
	}

	return nullptr;
}

bool ABossCore::CanReceiveCoreDamage() const
{
	const APawn_CompositeMaster* OwningCompositeMaster = GetOwningCompositeMaster();
	if (!IsValid(OwningCompositeMaster))
	{
		return true;
	}

	TArray<AActor*> SupportPartActors;
	OwningCompositeMaster->GetSupportPartActors(SupportPartActors);

	for (AActor* SupportPartActor : SupportPartActors)
	{
		const ABossOutPart* SupportPart = Cast<ABossOutPart>(SupportPartActor);
		if (IsValid(SupportPart) && !SupportPart->IsPartDefeated())
		{
			return false;
		}
	}

	return true;
}
