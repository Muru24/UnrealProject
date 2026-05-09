#include "SnakeSkillManager.h"

#include "Components/ChildActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealProject/P_Player.h"
#include "SnakeBodyChargeComponent.h"
#include "Snake_CompositeMaster.h"
#include "SquadCraftActor.h"
#include "SquadRuntimeComponent.h"

namespace
{
	AActor* ResolveRandomPlayerCraftTarget(UWorld* World)
	{
		AP_Player* PlayerPawn = World ? Cast<AP_Player>(UGameplayStatics::GetPlayerPawn(World, 0)) : nullptr;
		if (!IsValid(PlayerPawn))
		{
			return World ? UGameplayStatics::GetPlayerPawn(World, 0) : nullptr;
		}

		if (USquadRuntimeComponent* SquadRuntimeComponent = PlayerPawn->FindComponentByClass<USquadRuntimeComponent>())
		{
			TArray<ASquadCraftActor*> SquadCrafts;
			SquadRuntimeComponent->GetAllCrafts(SquadCrafts);

			TArray<ASquadCraftActor*> OperationalCrafts;
			for (ASquadCraftActor* Craft : SquadCrafts)
			{
				if (IsValid(Craft) && Craft->IsOperational())
				{
					OperationalCrafts.Add(Craft);
				}
			}

			if (!OperationalCrafts.IsEmpty())
			{
				return OperationalCrafts[FMath::RandRange(0, OperationalCrafts.Num() - 1)];
			}
		}

		return PlayerPawn;
	}
}

USnakeSkillManager::USnakeSkillManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	SkillAutoTriggerTimer = 10.0f;
}

void USnakeSkillManager::Update(float DeltaTime, const TArray<UChildActorComponent*>& Segments)
{
	ASnake_CompositeMaster* Master = Cast<ASnake_CompositeMaster>(GetOwner());
	if (!Master)
	{
		return;
	}

	if (!Master->RunPatten && !bIsSkillActive)
	{
		SkillAutoTriggerTimer -= DeltaTime;
		if (SkillAutoTriggerTimer <= 0.0f)
		{
			if (FMath::RandBool())
			{
				StartBodyChargeSkill(Segments);
			}
			else
			{
				StartLaserSkill(Segments);
			}
		}
	}

	if (!bIsSkillActive)
	{
		return;
	}

	bool bAllPartsIdle = true;
	bool bAllPartsFormed = true;

	for (UChildActorComponent* Segment : Segments)
	{
		if (!Segment || !Segment->GetChildActor())
		{
			continue;
		}

		if (USnakeBodyChargeComponent* SkillComp = Segment->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
		{
			const ESnakePartSkillState SkillState = SkillComp->GetCurrentState();
			if (SkillState != ESnakePartSkillState::Idle)
			{
				bAllPartsIdle = false;
			}

			if ((CurrentSkillPhase == ESnakeSkillState::Forming || CurrentSkillPhase == ESnakeSkillState::Laser_Preparing) &&
				!SkillComp->IsReachedFormation())
			{
				bAllPartsFormed = false;
			}
		}
	}

	if (CurrentSkillPhase == ESnakeSkillState::Forming && bAllPartsFormed)
	{
		for (UChildActorComponent* Segment : Segments)
		{
			if (!Segment || !Segment->GetChildActor())
			{
				continue;
			}

			if (USnakeBodyChargeComponent* SkillComp = Segment->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
			{
				SkillComp->SignalLaunch();
			}
		}

		CurrentSkillPhase = ESnakeSkillState::Attacking;
	}
	else if (CurrentSkillPhase == ESnakeSkillState::Attacking && bAllPartsIdle)
	{
		bIsSkillActive = false;
		CurrentSkillPhase = ESnakeSkillState::Idle;
		Master->RunPatten = false;
		SkillAutoTriggerTimer = SkillAutoTriggerDelay;
	}
	else if (CurrentSkillPhase == ESnakeSkillState::Laser_Preparing)
	{
		StateTimer -= DeltaTime;
		if (StateTimer <= 0.0f)
		{
			CurrentSkillPhase = ESnakeSkillState::Laser_Firing;
			CurrentFiringIndex = 0;
			SequentialTimer = 0.0f;
			StateTimer = LaserDuration;
		}
	}
	else if (CurrentSkillPhase == ESnakeSkillState::Laser_Firing)
	{
		if (CurrentFiringIndex < Segments.Num())
		{
			SequentialTimer -= DeltaTime;
			if (SequentialTimer <= 0.0f)
			{
				if (Segments[CurrentFiringIndex] && Segments[CurrentFiringIndex]->GetChildActor())
				{
					if (USnakeBodyChargeComponent* SkillComp = Segments[CurrentFiringIndex]->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
					{
						SkillComp->FireLaser();
					}
				}

				CurrentFiringIndex++;
				SequentialTimer = LaunchDelayBetweenParts;
			}
		}

		StateTimer -= DeltaTime;
		if (StateTimer <= 0.0f && CurrentFiringIndex >= Segments.Num())
		{
			for (UChildActorComponent* Segment : Segments)
			{
				if (!Segment || !Segment->GetChildActor())
				{
					continue;
				}

				if (USnakeBodyChargeComponent* SkillComp = Segment->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
				{
					SkillComp->StopLaser();
				}
			}

			bIsSkillActive = false;
			CurrentSkillPhase = ESnakeSkillState::Idle;
			Master->RunPatten = false;
			SkillAutoTriggerTimer = SkillAutoTriggerDelay;
		}
	}
}

void USnakeSkillManager::StartBodyChargeSkill(const TArray<UChildActorComponent*>& Segments)
{
	ASnake_CompositeMaster* Master = Cast<ASnake_CompositeMaster>(GetOwner());
	if (!Master || Segments.Num() == 0 || bIsSkillActive)
	{
		return;
	}

	AActor* TargetActor = ResolveRandomPlayerCraftTarget(GetWorld());
	if (!IsValid(TargetActor))
	{
		return;
	}

	bIsSkillActive = true;
	Master->RunPatten = true;
	CurrentSkillPhase = ESnakeSkillState::Forming;

	for (int32 i = 0; i < Segments.Num(); ++i)
	{
		if (!Segments[i] || !Segments[i]->GetChildActor())
		{
			continue;
		}

		if (USnakeBodyChargeComponent* SkillComp = Segments[i]->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
		{
			const float AngleRad = FMath::DegreesToRadians((360.0f / FMath::Max(1, Segments.Num())) * i);
			const FVector CircleOffset(FMath::Cos(AngleRad) * FormationRadius, FMath::Sin(AngleRad) * FormationRadius, 0.0f);
			const FVector TargetFormPos = Master->GetActorLocation() + Master->GetActorRotation().RotateVector(CircleOffset);
			SkillComp->InitSkillSequence(TargetActor, TargetFormPos, Master->GetActorRotation(), i * LaunchDelayBetweenParts);
		}
	}
}

void USnakeSkillManager::StopAllSkills(const TArray<UChildActorComponent*>& Segments)
{
	bIsSkillActive = false;
	CurrentSkillPhase = ESnakeSkillState::Idle;
	StateTimer = 0.0f;
	SequentialTimer = 0.0f;
	CurrentFiringIndex = 0;
	SkillAutoTriggerTimer = SkillAutoTriggerDelay;

	if (ASnake_CompositeMaster* Master = Cast<ASnake_CompositeMaster>(GetOwner()))
	{
		Master->RunPatten = false;
	}

	for (UChildActorComponent* Segment : Segments)
	{
		if (!Segment || !Segment->GetChildActor())
		{
			continue;
		}

		if (USnakeBodyChargeComponent* SkillComp = Segment->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
		{
			SkillComp->CancelSkillSequence();
		}
	}
}

void USnakeSkillManager::StartLaserSkill(const TArray<UChildActorComponent*>& Segments)
{
	ASnake_CompositeMaster* Master = Cast<ASnake_CompositeMaster>(GetOwner());
	if (!Master || Segments.Num() == 0 || bIsSkillActive)
	{
		return;
	}

	AActor* TargetActor = ResolveRandomPlayerCraftTarget(GetWorld());
	if (!IsValid(TargetActor))
	{
		return;
	}

	bIsSkillActive = true;
	Master->RunPatten = true;
	CurrentSkillPhase = ESnakeSkillState::Laser_Preparing;
	StateTimer = LaserPrepareTime;

	const FVector BaseLoc = Master->GetActorLocation() + FVector(0.0f, 0.0f, LaserHeightOffset);
	const FVector RightDir = Master->GetActorRightVector();

	for (int32 i = 0; i < Segments.Num(); ++i)
	{
		if (!Segments[i] || !Segments[i]->GetChildActor())
		{
			continue;
		}

		if (USnakeBodyChargeComponent* SkillComp = Segments[i]->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
		{
			const float Offset = (i - (Segments.Num() / 2.0f)) * LaserLineSpacing;
			const FVector TargetPos = BaseLoc + (RightDir * Offset);
			SkillComp->PrepareLaser(TargetActor, TargetPos, Master->GetActorRotation());
		}
	}
}
