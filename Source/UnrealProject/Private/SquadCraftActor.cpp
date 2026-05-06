#include "SquadCraftActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CraftAttackComponent.h"
#include "CraftLoadoutComponent.h"
#include "SkillComponent.h"

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
	CraftMesh->SetHiddenInGame(false);
	CraftMesh->SetVisibility(true);

	FireOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("FireOrigin"));
	FireOrigin->SetupAttachment(VisualRoot);
	FireOrigin->SetRelativeLocation(FVector(120.0f, 0.0f, 0.0f));

	AttackComponent = CreateDefaultSubobject<UCraftAttackComponent>(TEXT("AttackComponent"));
	LoadoutComponent = CreateDefaultSubobject<UCraftLoadoutComponent>(TEXT("LoadoutComponent"));
	SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
}

void ASquadCraftActor::BeginPlay()
{
	Super::BeginPlay();

	ApplyLoadout();

	if (GetRootComponent())
	{
		GetRootComponent()->SetRelativeLocation(DesiredRelativeLocation);
		GetRootComponent()->SetRelativeRotation(DesiredRelativeRotation);
	}

	if (VisualRoot)
	{
		VisualRoot->SetRelativeScale3D(bIsActiveCraft ? ActiveScale : InactiveScale);
	}

	if (CraftMesh)
	{
		CraftMesh->SetHiddenInGame(false);
		CraftMesh->SetVisibility(true, true);
		CraftMesh->SetRelativeRotation(DesiredMeshTiltRotation);
	}
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

	if (VisualRoot)
	{
		const FVector TargetScale = bIsActiveCraft ? ActiveScale : InactiveScale;
		VisualRoot->SetRelativeScale3D(
			FMath::VInterpTo(VisualRoot->GetRelativeScale3D(), TargetScale, DeltaTime, TransformInterpSpeed));
	}

	if (CraftMesh)
	{
		CraftMesh->SetRelativeRotation(
			FMath::RInterpTo(CraftMesh->GetRelativeRotation(), DesiredMeshTiltRotation, DeltaTime, TransformInterpSpeed));
	}
}

void ASquadCraftActor::ApplyLoadout()
{
	if (LoadoutComponent && AttackComponent)
	{
		LoadoutComponent->ApplyLoadoutToAttackComponent(AttackComponent);
		LoadoutComponent->ApplyLoadoutToSkillComponent(SkillComponent);
	}
}

ECraftCombatRole ASquadCraftActor::GetCombatRole() const
{
	return LoadoutComponent ? LoadoutComponent->GetCombatRole() : ECraftCombatRole::SupportRapid;
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

void ASquadCraftActor::SetVisualTiltRotation(const FRotator& InRotation)
{
	DesiredMeshTiltRotation = InRotation;
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

bool ASquadCraftActor::TryActivateOffensiveSkill(AActor* TargetActor)
{
	return bIsActiveCraft && SkillComponent
		? SkillComponent->TryActivateOffensiveSkill(TargetActor)
		: false;
}

bool ASquadCraftActor::TryActivateBuffSkill(AActor* TargetActor)
{
	// 버프 스킬은 기체의 활성화 여부와 관계없이 사용할 수 있도록 수정 (자동 발동 대응)
	return SkillComponent ? SkillComponent->TryActivateBuffSkill(TargetActor) : false;
}
