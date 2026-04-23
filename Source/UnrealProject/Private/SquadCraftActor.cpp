#include "SquadCraftActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CraftAttackComponent.h"

ASquadCraftActor::ASquadCraftActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(SceneRoot);

	CraftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CraftMesh"));
	CraftMesh->SetupAttachment(VisualRoot);
	CraftMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FireOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("FireOrigin"));
	FireOrigin->SetupAttachment(VisualRoot);
	FireOrigin->SetRelativeLocation(FVector(120.0f, 0.0f, 0.0f));

	AttackComponent = CreateDefaultSubobject<UCraftAttackComponent>(TEXT("AttackComponent"));
}

void ASquadCraftActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetRootComponent())
	{
		GetRootComponent()->SetRelativeLocation(DesiredRelativeLocation);
		GetRootComponent()->SetRelativeRotation(DesiredRelativeRotation);
	}

	VisualRoot->SetRelativeScale3D(bIsActiveCraft ? ActiveScale : InactiveScale);
}

void ASquadCraftActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetRootComponent())
	{
		GetRootComponent()->SetRelativeLocation(
			FMath::VInterpTo(GetCurrentRelativeLocation(), DesiredRelativeLocation, DeltaTime, TransformInterpSpeed));
		GetRootComponent()->SetRelativeRotation(
			FMath::RInterpTo(GetCurrentRelativeRotation(), DesiredRelativeRotation, DeltaTime, TransformInterpSpeed));
	}

	const FVector TargetScale = bIsActiveCraft ? ActiveScale : InactiveScale;
	VisualRoot->SetRelativeScale3D(FMath::VInterpTo(VisualRoot->GetRelativeScale3D(), TargetScale, DeltaTime, TransformInterpSpeed));
}

void ASquadCraftActor::SetActiveCraft(bool bInIsActiveCraft)
{
	bIsActiveCraft = bInIsActiveCraft;
}

void ASquadCraftActor::SetDesiredRelativeTransform(const FVector& InLocation, const FRotator& InRotation)
{
	DesiredRelativeLocation = InLocation;
	DesiredRelativeRotation = InRotation;
}

FRotator ASquadCraftActor::GetCurrentRelativeRotation() const
{
	return GetRootComponent() ? GetRootComponent()->GetRelativeRotation() : FRotator::ZeroRotator;
}

FVector ASquadCraftActor::GetCurrentRelativeLocation() const
{
	return GetRootComponent() ? GetRootComponent()->GetRelativeLocation() : FVector::ZeroVector;
}

bool ASquadCraftActor::FireAt(const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	return AttackComponent ? AttackComponent->FireFromOrigin(FireOrigin, TargetPoint, TargetActor, InstigatorPawn) : false;
}

bool ASquadCraftActor::TryAutoFireAt(const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	return !bIsActiveCraft && AttackComponent
		? AttackComponent->TryAutoFireFromOrigin(FireOrigin, TargetPoint, TargetActor, InstigatorPawn)
		: false;
}
