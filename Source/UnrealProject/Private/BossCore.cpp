#include "BossCore.h"

#include "BossPartPatternCoordinatorComponent.h"
#include "HUDManager.h"
#include "Kismet/GameplayStatics.h"
#include "Pawn_CompositeMaster.h"
#include "Snake_CompositeMaster.h"
#include "StatComponent.h"

ABossCore::ABossCore()
{
	PrimaryActorTick.bCanEverTick = false;
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

	if (APawn_CompositeMaster* OwningCompositeMaster = GetOwningCompositeMaster())
	{
		OwningCompositeMaster->StopAllBossCombat();
	}

	if (SpawnedSnake)
	{
		SpawnedSnake->StopAllBossCombat();
		SpawnedSnake->SetActorTickEnabled(false);
	}

	if (bHasDissolveParameter && DynamicMaterial)
	{
		StartDissolveOut();
	}
	else
	{
		RequestBossDeathSequence();
		RequestGameClear();
	}
}

void ABossCore::OnDissolveOutFinished()
{
	Super::OnDissolveOutFinished();
	RequestBossDeathSequence();
	RequestGameClear();
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
