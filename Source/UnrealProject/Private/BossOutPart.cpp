#include "BossOutPart.h"

#include "BossOutPartPatternComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CraftAttackComponent.h"
#include "HUDManager.h"
#include "Kismet/GameplayStatics.h"
#include "LaserAttackComponent.h"
#include "Pawn_CompositeMaster.h"
#include "StatComponent.h"

ABossOutPart::ABossOutPart()
{
	PrimaryActorTick.bCanEverTick = true;

	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(RootComponent);

	if (PlaneMesh)
	{
		PlaneMesh->SetupAttachment(VisualRoot);
	}

	FrontFireOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("FrontFireOrigin"));
	FrontFireOrigin->SetupAttachment(VisualRoot);
	FrontFireOrigin->SetRelativeLocation(FVector(160.0f, 0.0f, 0.0f));

	SideFireOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("SideFireOrigin"));
	SideFireOrigin->SetupAttachment(VisualRoot);
	SideFireOrigin->SetRelativeLocation(FVector(0.0f, 160.0f, 0.0f));

	CommonAttackComponent = CreateDefaultSubobject<UCraftAttackComponent>(TEXT("CommonAttackComponent"));
	LaserAttackComponent = CreateDefaultSubobject<ULaserAttackComponent>(TEXT("LaserAttackComponent"));
	PatternComponent = CreateDefaultSubobject<UBossOutPartPatternComponent>(TEXT("PatternComponent"));

	DesiredVisualRotation = DefaultVisualRotation;
}

void ABossOutPart::BeginPlay()
{
	Super::BeginPlay();

	if (StatComponent)
	{
		StatComponent->OnHpChanged.AddDynamic(this, &ABossOutPart::HandleHpChanged);
	}

	DesiredVisualRotation = bUseSideAttackPose ? SideAttackVisualRotation : DefaultVisualRotation;

	if (VisualRoot)
	{
		VisualRoot->SetRelativeRotation(DesiredVisualRotation);
	}

	if (LaserAttackComponent)
	{
		LaserAttackComponent->SetFireOrigin(SideFireOrigin);
	}
}

void ABossOutPart::HandleHpChanged(float CurrentHp)
{
	if (CurrentHp > 0.0f || bPartDestroyedHandled)
	{
		return;
	}

	HandlePartDestroyed();
}

void ABossOutPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!VisualRoot)
	{
		return;
	}

	VisualRoot->SetRelativeRotation(
		FMath::RInterpTo(
			VisualRoot->GetRelativeRotation(),
			DesiredVisualRotation,
			DeltaTime,
			PoseInterpSpeed));
}

void ABossOutPart::SetUseSideAttackPose(bool bEnableSideAttackPose)
{
	bUseSideAttackPose = bEnableSideAttackPose;
	DesiredVisualRotation = bUseSideAttackPose ? SideAttackVisualRotation : DefaultVisualRotation;
}

void ABossOutPart::HandlePartDestroyed()
{
	if (bPartDestroyedHandled)
	{
		return;
	}

	bPartDestroyedHandled = true;

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (AHUDManager* HUDManager = Cast<AHUDManager>(PlayerController->GetHUD()))
		{
			if (PatternComponent && HUDManager->IsMiniGameOwnedByPattern(PatternComponent))
			{
				HUDManager->ForceFinishMiniGame(true);
			}
		}
	}

	if (APawn_CompositeMaster* OwningCompositeMaster = GetOwningCompositeMaster())
	{
		OwningCompositeMaster->StopAllBossCombat();
		OwningCompositeMaster->RequestCombatRestartSequence();
	}

	RequestPartDestroyedSequence();
}

void ABossOutPart::RequestPartDestroyedSequence()
{
	ReceivePartDestroyedSequenceRequested();
}

APawn_CompositeMaster* ABossOutPart::GetOwningCompositeMaster() const
{
	if (const USceneComponent* ParentSceneComponent = GetParentComponent())
	{
		return Cast<APawn_CompositeMaster>(ParentSceneComponent->GetOwner());
	}

	return nullptr;
}

USceneComponent* ABossOutPart::GetActiveFireOrigin() const
{
	return bUseSideAttackPose ? SideFireOrigin : FrontFireOrigin;
}
