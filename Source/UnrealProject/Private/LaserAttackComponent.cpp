#include "LaserAttackComponent.h"

#include "BeamEffectActor.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "Engine/World.h"

ULaserAttackComponent::ULaserAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULaserAttackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopLaser();
	Super::EndPlay(EndPlayReason);
}

void ULaserAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bWarningActive && !bAttackActive)
	{
		return;
	}

	if (!bHasLockedAimLocation)
	{
		StopLaser();
		return;
	}

	const FVector AimLocation = ResolveCurrentAimLocation();

	if (bWarningActive)
	{
		if (!IsValid(ActiveBeamActor))
		{
			return;
		}

		WarningTimeTracker += DeltaTime;
		const float WarningTimeRemaining = FMath::Max(0.0f, WarningDuration - WarningTimeTracker);
		const bool bUseFinalPulse = WarningFinalPulseDuration > 0.0f && WarningTimeRemaining <= WarningFinalPulseDuration;
		const FLinearColor CurrentWarningColor = bUseFinalPulse ? WarningFinalPulseColor : WarningBeamColor;
		const float CurrentWarningSize = bUseFinalPulse
			? WarningBeamSize * WarningFinalPulseSizeMultiplier
			: WarningBeamSize;

		ActiveBeamActor->SetActorLocation(GetFireOriginLocation());
		ActiveBeamActor->SetBeamEnd(AimLocation, true);
		ActiveBeamActor->SetMainColor(CurrentWarningColor, true);
		ActiveBeamActor->SetBeamSize(CurrentWarningSize, true);
	}
	else if (bAttackActive)
	{
		if (!IsValid(ActiveBeamActor))
		{
			return;
		}

		AttackTimeTracker += DeltaTime;
		CurrentBeamAimLocation = FMath::VInterpTo(CurrentBeamAimLocation, AimLocation, DeltaTime, AttackTrackingInterpSpeed);
		ActiveBeamActor->SetActorLocation(GetFireOriginLocation());
		ActiveBeamActor->SetBeamEnd(CurrentBeamAimLocation, true);

		const float Alpha = AttackActiveDuration > 0.0f
			? FMath::Clamp(AttackTimeTracker / AttackActiveDuration, 0.0f, 1.0f)
			: 0.0f;
		const float CurrentSize = FMath::Lerp(InitialBeamSize, FinalBeamSize, Alpha);

		ActiveBeamActor->SetMainColor(AttackBeamColor, true);
		ActiveBeamActor->SetBeamSize(CurrentSize, true);

		if (AActor* Owner = GetOwner())
		{
			const FVector Direction = CurrentBeamAimLocation - Owner->GetActorLocation();
			if (!Direction.IsNearlyZero())
			{
				Owner->SetActorRotation(Direction.Rotation());
			}
		}
	}
}

void ULaserAttackComponent::StartLaser(AActor* InTarget)
{
	if (!IsValid(InTarget))
	{
		return;
	}

	SetTargetActor(InTarget);
	BeginLaserWarning();
}

void ULaserAttackComponent::StartLaserFromComponent(USceneComponent* InTargetComponent)
{
	if (!IsValid(InTargetComponent))
	{
		return;
	}

	SetTargetComponent(InTargetComponent);
	BeginLaserWarning();
}

void ULaserAttackComponent::StopLaser()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WarningFinishTimerHandle);
		World->GetTimerManager().ClearTimer(AttackStopTimerHandle);
	}

	bWarningActive = false;
	bAttackActive = false;
	AttackTimeTracker = 0.0f;
	WarningTimeTracker = 0.0f;
	CurrentBeamAimLocation = FVector::ZeroVector;
	bHasLockedAimLocation = false;
	DeactivateActiveBeam();
}

void ULaserAttackComponent::SetTargetActor(AActor* InTargetActor)
{
	TargetActor = InTargetActor;
	TargetComponent = nullptr;
}

void ULaserAttackComponent::SetTargetComponent(USceneComponent* InTargetComponent)
{
	TargetComponent = InTargetComponent;
	TargetActor = nullptr;
}

void ULaserAttackComponent::FireBeam()
{
	if ((!bWarningActive && !bAttackActive) || !bHasLockedAimLocation || !BeamActorClass)
	{
		return;
	}

	if (!IsValid(ActiveBeamActor))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Cast<APawn>(GetOwner());
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ActiveBeamActor = GetWorld()->SpawnActor<ABeamEffectActor>(
			BeamActorClass,
			GetFireOriginLocation(),
			GetOwner() ? GetOwner()->GetActorRotation() : FRotator::ZeroRotator,
			SpawnParams);
	}

	if (!IsValid(ActiveBeamActor))
	{
		return;
	}

	ActiveBeamActor->SetActorLocation(GetFireOriginLocation());
	ActiveBeamActor->SetBeamEnd(ResolveCurrentAimLocation(), false);

	if (bWarningActive)
	{
		ActiveBeamActor->SetMainColor(WarningBeamColor, false);
		ActiveBeamActor->SetBeamSize(WarningBeamSize, false);
		ActiveBeamActor->SetBeamDamageEnabled(false);
	}
	else
	{
		ActiveBeamActor->SetMainColor(AttackBeamColor, false);
		ActiveBeamActor->SetBeamSize(InitialBeamSize, false);
		ActiveBeamActor->ConfigureBeamDamage(AttackDamage, AttackDamageTickInterval, AttackTraceRadius, true);
		ActiveBeamActor->SetBeamDamageEnabled(true);
	}

	ActiveBeamActor->ApplyBeamParameters();
	ActiveBeamActor->ActivateBeam(true);
}

void ULaserAttackComponent::BeginLaserWarning()
{
	StopLaser();

	if ((!IsValid(TargetActor) && !IsValid(TargetComponent)) || !BeamActorClass)
	{
		return;
	}

	bWarningActive = true;
	AttackTimeTracker = 0.0f;
	WarningTimeTracker = 0.0f;
	CurrentBeamAimLocation = GetTargetLocationWithExtension();
	bHasLockedAimLocation = true;
	FireBeam();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			WarningFinishTimerHandle,
			this,
			&ULaserAttackComponent::BeginLaserFiring,
			FMath::Max(0.0f, WarningDuration),
			false);
	}
}

void ULaserAttackComponent::BeginLaserFiring()
{
	if (!bWarningActive)
	{
		return;
	}

	if (!bHasLockedAimLocation)
	{
		StopLaser();
		return;
	}

	bWarningActive = false;
	bAttackActive = true;
	AttackTimeTracker = 0.0f;
	CurrentBeamAimLocation = ResolveCurrentAimLocation();
	FireBeam();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AttackStopTimerHandle);

		if (AttackActiveDuration > 0.0f)
		{
			World->GetTimerManager().SetTimer(
				AttackStopTimerHandle,
				this,
				&ULaserAttackComponent::StopLaser,
				AttackActiveDuration,
				false);
		}
	}
}

void ULaserAttackComponent::DeactivateActiveBeam()
{
	if (ActiveBeamActor)
	{
		ActiveBeamActor->DeactivateBeam();
		ActiveBeamActor->Destroy();
		ActiveBeamActor = nullptr;
	}
}

FVector ULaserAttackComponent::ResolveCurrentAimLocation() const
{
	return CurrentBeamAimLocation;
}

FVector ULaserAttackComponent::GetTargetLocationWithExtension() const
{
	FVector TargetLoc = FVector::ZeroVector;

	if (IsValid(TargetComponent))
	{
		TargetLoc = TargetComponent->GetComponentLocation();
	}
	else if (IsValid(TargetActor))
	{
		TargetLoc = TargetActor->GetActorLocation();
	}
	else
	{
		return FVector::ZeroVector;
	}

	const FVector Origin = GetFireOriginLocation();
	const FVector Direction = (TargetLoc - Origin).GetSafeNormal();

	return TargetLoc + (Direction * BeamExtraDistance);
}

FVector ULaserAttackComponent::GetFireOriginLocation() const
{
	return FireOrigin ? FireOrigin->GetComponentLocation() : (GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector);
}
