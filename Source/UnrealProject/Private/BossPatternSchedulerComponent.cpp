#include "BossPatternSchedulerComponent.h"

#include "BossOutPart.h"
#include "BossPartPatternCoordinatorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Pawn_CompositeMaster.h"

UBossPatternSchedulerComponent::UBossPatternSchedulerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBossPatternSchedulerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCompositeMaster = Cast<APawn_CompositeMaster>(GetOwner());
}

void UBossPatternSchedulerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bAutoPatternEnabled || !OwnerCompositeMaster)
	{
		return;
	}

	CommonPatternTimer += DeltaTime;
	if (CommonPatternTimer >= CommonPatternTickInterval)
	{
		CommonPatternTimer = 0.0f;
		TryRunCommonPatterns();
	}

	SpecialPatternTimer += DeltaTime;
	if (SpecialPatternTimer < SpecialPatternInterval)
	{
		return;
	}

	SpecialPatternTimer = 0.0f;
	TryRunNextSpecialPattern();
}

void UBossPatternSchedulerComponent::TryRunNextSpecialPattern()
{
	if (!OwnerCompositeMaster)
	{
		return;
	}

	UBossPartPatternCoordinatorComponent* Coordinator = OwnerCompositeMaster->GetBossPartPatternCoordinatorComponent();
	if (!Coordinator || Coordinator->GetActivePatternPart())
	{
		return;
	}

	TArray<AActor*> SupportPartActors;
	OwnerCompositeMaster->GetSupportPartActors(SupportPartActors);
	if (SupportPartActors.IsEmpty())
	{
		return;
	}

	TArray<ABossOutPart*> AvailableParts;
	for (AActor* SupportPartActor : SupportPartActors)
	{
		if (ABossOutPart* SupportPart = Cast<ABossOutPart>(SupportPartActor))
		{
			if (SupportPart->GetPatternComponent())
			{
				AvailableParts.Add(SupportPart);
			}
		}
	}

	if (AvailableParts.IsEmpty())
	{
		return;
	}

	const int32 RandomIndex = FMath::RandRange(0, AvailableParts.Num() - 1);
	Coordinator->TryExecuteSpecialPattern(AvailableParts[RandomIndex], ResolveTargetActor());
}

void UBossPatternSchedulerComponent::TryRunCommonPatterns()
{
	if (!OwnerCompositeMaster)
	{
		return;
	}

	UBossPartPatternCoordinatorComponent* Coordinator = OwnerCompositeMaster->GetBossPartPatternCoordinatorComponent();
	if (!Coordinator)
	{
		return;
	}

	TArray<AActor*> SupportPartActors;
	OwnerCompositeMaster->GetSupportPartActors(SupportPartActors);
	AActor* TargetActor = ResolveTargetActor();
	ABossOutPart* ActiveSpecialPart = Coordinator->GetActivePatternPart();

	for (AActor* SupportPartActor : SupportPartActors)
	{
		ABossOutPart* SupportPart = Cast<ABossOutPart>(SupportPartActor);
		if (!SupportPart || SupportPart == ActiveSpecialPart)
		{
			continue;
		}

		Coordinator->TryExecuteCommonPattern(SupportPart, TargetActor);
	}
}

AActor* UBossPatternSchedulerComponent::ResolveTargetActor() const
{
	return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}
