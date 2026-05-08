#include "SquadCraftActor.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CraftAttackComponent.h"
#include "CraftLoadoutComponent.h"
#include "SkillComponent.h"
#include "StatComponent.h"

ASquadCraftActor::ASquadCraftActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(SceneRoot);
	CollisionComponent->SetSphereRadius(90.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionProfileName(TEXT("Pawn"));
	CollisionComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

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
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
}

void ASquadCraftActor::BeginPlay()
{
	Super::BeginPlay();

	if (StatComponent)
	{
		StatComponent->OnHpChanged.AddDynamic(this, &ASquadCraftActor::HandleHpChanged);
	}

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

	if (bCraftDefeated)
	{
		return;
	}

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

void ASquadCraftActor::GetHealthInfo(float& OutCurrentHP, float& OutMaxHP) const
{
	if (StatComponent)
	{
		OutCurrentHP = StatComponent->Stats.unit.HP;
		OutMaxHP = StatComponent->Stats.unit.MaxHP;
	}
	else
	{
		OutCurrentHP = 0.0f;
		OutMaxHP = 0.0f;
	}
}

void ASquadCraftActor::GetSkillCooldowns(float& OutBuffCooldown, float& OutOffensiveCooldown) const
{
	if (SkillComponent)
	{
		OutBuffCooldown = SkillComponent->GetBuffSkillCooldownRemaining();
		OutOffensiveCooldown = SkillComponent->GetOffensiveSkillCooldownRemaining();
	}
	else
	{
		OutBuffCooldown = 0.0f;
		OutOffensiveCooldown = 0.0f;
	}
}

bool ASquadCraftActor::FireAt(const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	return !bCraftDefeated && AttackComponent
		? AttackComponent->FireFromOrigin(FireOrigin, TargetPoint, TargetActor, InstigatorPawn)
		: false;
}

bool ASquadCraftActor::TryAutoFireAt(const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn)
{
	return !bCraftDefeated && !bIsActiveCraft && AttackComponent
		? AttackComponent->TryAutoFireFromOrigin(FireOrigin, TargetPoint, TargetActor, InstigatorPawn)
		: false;
}

bool ASquadCraftActor::TryActivateOffensiveSkill(AActor* TargetActor)
{
	return !bCraftDefeated && bIsActiveCraft && SkillComponent
		? SkillComponent->TryActivateOffensiveSkill(TargetActor)
		: false;
}

bool ASquadCraftActor::TryActivateBuffSkill(AActor* TargetActor)
{
	return !bCraftDefeated && SkillComponent ? SkillComponent->TryActivateBuffSkill(TargetActor) : false;
}

void ASquadCraftActor::HandleHpChanged(float CurrentHp)
{
	if (!bCraftDefeated && CurrentHp <= 0.0f)
	{
		HandleCraftDefeated();
	}
}

void ASquadCraftActor::HandleCraftDefeated()
{
	if (bCraftDefeated)
	{
		return;
	}

	bCraftDefeated = true;
	bIsActiveCraft = false;

	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	OnCraftDefeated.Broadcast(this);
}
