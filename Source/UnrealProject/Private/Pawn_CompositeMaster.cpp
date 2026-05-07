#include "Pawn_CompositeMaster.h"

#include "BossOutPartPatternComponent.h"
#include "BossCore.h"
#include "BossOutPart.h"
#include "BossPartPatternCoordinatorComponent.h"
#include "BossPatternSchedulerComponent.h"
#include "BossSupportPartOrbitComponent.h"
#include "BulletBase.h"
#include "ChildActorPartsComponent.h"
#include "Components/ChildActorComponent.h"
#include "EngineUtils.h"
#include "SnakeBodyChargeComponent.h"

APawn_CompositeMaster::APawn_CompositeMaster()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	ChildActorPartsComponent = CreateDefaultSubobject<UChildActorPartsComponent>(TEXT("ChildActorPartsComponent"));
	BossPartPatternCoordinatorComponent = CreateDefaultSubobject<UBossPartPatternCoordinatorComponent>(TEXT("BossPartPatternCoordinatorComponent"));
	BossPatternSchedulerComponent = CreateDefaultSubobject<UBossPatternSchedulerComponent>(TEXT("BossPatternSchedulerComponent"));
	BossSupportPartOrbitComponent = CreateDefaultSubobject<UBossSupportPartOrbitComponent>(TEXT("BossSupportPartOrbitComponent"));
}

void APawn_CompositeMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APawn_CompositeMaster::BeginPlay()
{
	Super::BeginPlay();

	if (!ChildActorPartsComponent)
	{
		return;
	}

	TArray<UChildActorComponent*> GatheredChildParts;
	ChildActorPartsComponent->RefreshChildParts();
	ChildActorPartsComponent->GetChildParts(GatheredChildParts);

	ChildParts.Reset();
	for (UChildActorComponent* ChildPart : GatheredChildParts)
	{
		if (IsValid(ChildPart))
		{
			ChildParts.Add(ChildPart);
		}
	}
}

void APawn_CompositeMaster::GetChildPartActors(TArray<AActor*>& OutChildPartActors) const
{
	if (!ChildActorPartsComponent)
	{
		OutChildPartActors.Reset();
		return;
	}

	ChildActorPartsComponent->GetChildPartActors(OutChildPartActors);
}

void APawn_CompositeMaster::GetChildPartComponents(TArray<UChildActorComponent*>& OutChildPartComponents) const
{
	if (!ChildActorPartsComponent)
	{
		OutChildPartComponents.Reset();
		return;
	}

	ChildActorPartsComponent->GetChildParts(OutChildPartComponents);
}

void APawn_CompositeMaster::GetSupportPartActors(TArray<AActor*>& OutSupportPartActors) const
{
	OutSupportPartActors.Reset();

	TArray<AActor*> ChildPartActors;
	GetChildPartActors(ChildPartActors);

	for (AActor* ChildPartActor : ChildPartActors)
	{
		if (ABossOutPart* SupportPart = Cast<ABossOutPart>(ChildPartActor))
		{
			OutSupportPartActors.Add(SupportPart);
		}
	}
}

ABossCore* APawn_CompositeMaster::GetBossCore() const
{
	TArray<AActor*> ChildPartActors;
	GetChildPartActors(ChildPartActors);

	for (AActor* ChildPartActor : ChildPartActors)
	{
		if (ABossCore* BossCore = Cast<ABossCore>(ChildPartActor))
		{
			return BossCore;
		}
	}

	return nullptr;
}

void APawn_CompositeMaster::StopAllBossCombat()
{
	if (BossPartPatternCoordinatorComponent)
	{
		BossPartPatternCoordinatorComponent->StopCurrentPattern();
	}

	if (BossPatternSchedulerComponent)
	{
		BossPatternSchedulerComponent->SetAutoPatternEnabled(false);
		BossPatternSchedulerComponent->SetComponentTickEnabled(false);
	}

	if (BossSupportPartOrbitComponent)
	{
		BossSupportPartOrbitComponent->SetComponentTickEnabled(false);
	}

	TArray<AActor*> ChildPartActors;
	GetChildPartActors(ChildPartActors);
	for (AActor* ChildPartActor : ChildPartActors)
	{
		if (!IsValid(ChildPartActor))
		{
			continue;
		}

		if (UBossOutPartPatternComponent* BossOutPartPatternComponent = ChildPartActor->FindComponentByClass<UBossOutPartPatternComponent>())
		{
			BossOutPartPatternComponent->StopActivePattern();
			BossOutPartPatternComponent->SetComponentTickEnabled(false);
		}

		if (USnakeBodyChargeComponent* SnakeBodyChargeComponent = ChildPartActor->FindComponentByClass<USnakeBodyChargeComponent>())
		{
			SnakeBodyChargeComponent->CancelSkillSequence();
			SnakeBodyChargeComponent->SetComponentTickEnabled(false);
		}

		ChildPartActor->SetActorTickEnabled(false);
	}

	if (!GetWorld())
	{
		return;
	}

	TArray<ABulletBase*> ActiveBullets;
	for (TActorIterator<ABulletBase> BulletIterator(GetWorld()); BulletIterator; ++BulletIterator)
	{
		if (ABulletBase* BulletActor = *BulletIterator)
		{
			ActiveBullets.Add(BulletActor);
		}
	}

	for (ABulletBase* BulletActor : ActiveBullets)
	{
		if (IsValid(BulletActor))
		{
			BulletActor->Destroy();
		}
	}
}

void APawn_CompositeMaster::RequestCombatRestartSequence()
{
	ReceiveCombatRestartSequenceRequested();
}
