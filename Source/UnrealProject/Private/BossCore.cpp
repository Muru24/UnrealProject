#include "BossCore.h"

#include "Snake_CompositeMaster.h"

ABossCore::ABossCore()
{
	PrimaryActorTick.bCanEverTick = false;
	BossPhaseComponent = CreateDefaultSubobject<UBossPhaseComponent>(TEXT("BossPhaseComponent"));
}

void ABossCore::BeginPlay()
{
	Super::BeginPlay();

	if (BossPhaseComponent && StatComponent)
	{
		BossPhaseComponent->InitializePhaseTracking(StatComponent);
		BossPhaseComponent->OnBossPhaseChanged.AddDynamic(this, &ABossCore::HandleBossPhaseChanged);
	}

	if (!SnakeClass)
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

	if (BossPhaseComponent)
	{
		SpawnedSnake->ApplyBossPhase(BossPhaseComponent->GetCurrentPhase());
		BossPhaseComponent->StartEncounter();
		SpawnedSnake->ApplyBossPhase(BossPhaseComponent->GetCurrentPhase());
	}
}

void ABossCore::HandleBossPhaseChanged(EBossEncounterPhase PreviousPhase, EBossEncounterPhase NewPhase)
{
	if (SpawnedSnake)
	{
		SpawnedSnake->ApplyBossPhase(NewPhase);
	}
}
