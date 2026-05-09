#include "BeamEffectActor.h"

#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "StatComponent.h"
#include "UObject/ConstructorHelpers.h"

ABeamEffectActor::ABeamEffectActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BeamComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Beam"));
	BeamComponent->SetupAttachment(SceneRoot);
	BeamComponent->SetAutoActivate(true);

	HitComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Hit"));
	HitComponent->SetupAttachment(SceneRoot);
	HitComponent->SetAutoActivate(false);
	HitComponent->SetRelativeScale3D(FVector(1.2f));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DefaultBeamSystemRef(
		TEXT("/Game/StylizedToonBeam01/Particles/NS_ky_beam_fire_01.NS_ky_beam_fire_01"));
	if (DefaultBeamSystemRef.Succeeded())
	{
		DefaultBeamSystem = DefaultBeamSystemRef.Object;
		BeamComponent->SetAsset(DefaultBeamSystem);
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DefaultHitSystemRef(
		TEXT("/Game/StylizedToonBeam01/Particles/NS_ky_hit_fire_01.NS_ky_hit_fire_01"));
	if (DefaultHitSystemRef.Succeeded())
	{
		DefaultHitSystem = DefaultHitSystemRef.Object;
		HitComponent->SetAsset(DefaultHitSystem);
	}
}

void ABeamEffectActor::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultBeamSystem)
	{
		BeamComponent->SetAsset(DefaultBeamSystem);
	}

	if (DefaultHitSystem)
	{
		HitComponent->SetAsset(DefaultHitSystem);
	}

	ApplyBeamParameters();
}

void ABeamEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bBeamDamageEnabled || BeamDamage <= 0.0f || BeamDamageTickInterval < 0.0f)
	{
		return;
	}

	BeamDamageTickAccumulator += DeltaTime;
	if (BeamDamageTickAccumulator < BeamDamageTickInterval)
	{
		return;
	}

	BeamDamageTickAccumulator = 0.0f;
	ApplyBeamDamage();
}

void ABeamEffectActor::ApplyBeamParameters()
{
	ApplyBeamColor();
	ApplyBeamEnd();
	ApplyBeamSize();
}

void ABeamEffectActor::ApplyBeamColor()
{
	if (BeamComponent && MainColorParameterName != NAME_None)
	{
		BeamComponent->SetVariableLinearColor(MainColorParameterName, MainColor);
	}
}

void ABeamEffectActor::ApplyBeamEnd()
{
	if (BeamComponent && BeamEndParameterName != NAME_None)
	{
		const FVector BeamEndValue = bBeamEndUsesWorldSpace
			? BeamEnd
			: BeamComponent->GetComponentTransform().InverseTransformPosition(BeamEnd);
		BeamComponent->SetVariableVec3(BeamEndParameterName, BeamEndValue);
	}
}

void ABeamEffectActor::ApplyBeamSize()
{
	if (BeamComponent && BeamSizeParameterName != NAME_None)
	{

		BeamComponent->SetVariableFloat(BeamSizeParameterName, BeamSize);
	}
}

void ABeamEffectActor::SetMainColor(const FLinearColor& InMainColor, bool bApplyImmediately)
{
	MainColor = InMainColor;

	if (bApplyImmediately)
	{
		ApplyBeamColor();
	}
}

void ABeamEffectActor::SetBeamEnd(const FVector& InBeamEnd, bool bApplyImmediately)
{
	BeamEnd = InBeamEnd;

	if (bApplyImmediately)
	{
		ApplyBeamEnd();
	}
}

void ABeamEffectActor::SetBeamSize(float InBeamSize, bool bApplyImmediately)
{
	BeamSize = FMath::Max(0.0f, InBeamSize);

	if (bApplyImmediately)
	{
		ApplyBeamSize();
	}
}

void ABeamEffectActor::SetBeamSystem(UNiagaraSystem* InBeamSystem, bool bReinitialize)
{
	DefaultBeamSystem = InBeamSystem;

	if (BeamComponent)
	{
		BeamComponent->SetAsset(InBeamSystem);

		if (bReinitialize)
		{
			BeamComponent->ReinitializeSystem();
			ApplyBeamParameters();
		}
	}
}

void ABeamEffectActor::SetHitSystem(UNiagaraSystem* InHitSystem, bool bReinitialize)
{
	DefaultHitSystem = InHitSystem;

	if (HitComponent)
	{
		HitComponent->SetAsset(InHitSystem);

		if (bReinitialize)
		{
			HitComponent->ReinitializeSystem();
		}
	}
}

void ABeamEffectActor::ActivateBeam(bool bReset)
{
	if (!BeamComponent)
	{
		return;
	}

	ApplyBeamParameters();
	BeamDamageTickAccumulator = 0.0f;
	DamagedActorsThisActivation.Reset();
	BeamComponent->Activate(bReset);
}

void ABeamEffectActor::DeactivateBeam()
{
	if (BeamComponent)
	{
		BeamComponent->Deactivate();
	}

	BeamDamageTickAccumulator = 0.0f;
	DamagedActorsThisActivation.Reset();
}

void ABeamEffectActor::ActivateHitEffect(bool bReset)
{
	if (HitComponent)
	{
		HitComponent->Activate(bReset);
	}
}

void ABeamEffectActor::DeactivateHitEffect()
{
	if (HitComponent)
	{
		HitComponent->Deactivate();
	}
}

void ABeamEffectActor::SetBeamDamageEnabled(bool bEnabled)
{
	bBeamDamageEnabled = bEnabled;
	if (!bBeamDamageEnabled)
	{
		BeamDamageTickAccumulator = 0.0f;
		DamagedActorsThisActivation.Reset();
	}
}

void ABeamEffectActor::ConfigureBeamDamage(float InDamage, float InTickInterval, float InTraceRadius, bool bInAllowRepeatedDamageToSameTarget)
{
	BeamDamage = FMath::Max(0.0f, InDamage);
	BeamDamageTickInterval = FMath::Max(0.0f, InTickInterval);
	BeamTraceRadius = FMath::Max(0.0f, InTraceRadius);
	bAllowRepeatedDamageToSameTarget = bInAllowRepeatedDamageToSameTarget;
}

void ABeamEffectActor::ApplyBeamDamage()
{
	if (!GetWorld())
	{
		return;
	}

	const FVector StartLocation = GetActorLocation();
	const FVector EndLocation = BeamEnd;
	if (StartLocation.Equals(EndLocation, KINDA_SMALL_NUMBER))
	{
		return;
	}

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BeamDamageTrace), false, this);
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());

	if (AActor* OwnerActor = GetOwner())
	{
		QueryParams.AddIgnoredActor(OwnerActor->GetOwner());
	}

	const FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams::AllDynamicObjects;
	const bool bHitSomething = GetWorld()->SweepMultiByObjectType(
		HitResults,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(BeamTraceRadius),
		QueryParams);

	if (!bHitSomething)
	{
		return;
	}

	for (const FHitResult& HitResult : HitResults)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			TryApplyDamageToActor(HitActor);
		}
	}
}

bool ABeamEffectActor::TryApplyDamageToActor(AActor* TargetActor)
{
	if (!CanDamageActor(TargetActor))
	{
		return false;
	}

	if (!bAllowRepeatedDamageToSameTarget && DamagedActorsThisActivation.Contains(TargetActor))
	{
		return false;
	}

	if (UStatComponent* TargetStatComponent = TargetActor->FindComponentByClass<UStatComponent>())
	{
		TargetStatComponent->ApplyDamage(BeamDamage);
		if (!bAllowRepeatedDamageToSameTarget)
		{
			DamagedActorsThisActivation.Add(TargetActor);
		}
		return true;
	}

	return false;
}

bool ABeamEffectActor::CanDamageActor(AActor* OtherActor) const
{
	if (!IsValid(OtherActor) || OtherActor == this)
	{
		return false;
	}

	if (AActor* OwnerActor = GetOwner())
	{
		if (OtherActor == OwnerActor || OtherActor == OwnerActor->GetOwner())
		{
			return false;
		}
	}

	return true;
}
