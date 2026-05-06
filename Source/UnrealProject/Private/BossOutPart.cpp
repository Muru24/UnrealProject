#include "BossOutPart.h"

#include "BossOutPartPatternComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CraftAttackComponent.h"
#include "LaserAttackComponent.h"

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

USceneComponent* ABossOutPart::GetActiveFireOrigin() const
{
	return bUseSideAttackPose ? SideFireOrigin : FrontFireOrigin;
}
