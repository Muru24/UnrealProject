// Fill out your copyright notice in the Description page of Project Settings.

#include "Funnel.h"

#include "BeamEffectActor.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

AFunnel::AFunnel()
{
	FireOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("FireOrigin"));
	FireOrigin->SetupAttachment(RootComponent);

	if (!BeamActorClass)
	{
		BeamActorClass = ABeamEffectActor::StaticClass();
	}
}

void AFunnel::BeginPlay()
{
	Super::BeginPlay();

	OrbitAngleDegrees = FMath::FRandRange(0.0f, 360.0f);
	RuntimeOrbitRadius = OrbitRadius;
	RuntimeOrbitHeight = OrbitHeight;
	RuntimeOrbitAngularSpeed = OrbitAngularSpeed;
	RuntimeDesiredDistanceToTarget = DesiredDistanceToTarget;

	if (bRandomizeOrbitPerInstance)
	{
		RuntimeOrbitRadius = FMath::Max(0.0f, OrbitRadius + FMath::FRandRange(-OrbitRadiusRandomOffset, OrbitRadiusRandomOffset));
		RuntimeOrbitHeight = OrbitHeight + FMath::FRandRange(-OrbitHeightRandomOffset, OrbitHeightRandomOffset);
		RuntimeOrbitAngularSpeed = FMath::Max(0.0f, OrbitAngularSpeed + FMath::FRandRange(-OrbitSpeedRandomOffset, OrbitSpeedRandomOffset));
		RuntimeDesiredDistanceToTarget = FMath::Max(
			0.0f,
			DesiredDistanceToTarget + FMath::FRandRange(-DistanceRandomOffset, DistanceRandomOffset));

		if (FMath::RandBool())
		{
			RuntimeOrbitAngularSpeed *= -1.0f;
		}
	}

	if (bAutoStartBeamAttackOnBeginPlay && IsValid(TargetActor))
	{
		StartBeamAttack(TargetActor);
	}
}

void AFunnel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(TargetActor))
	{
		StopBeamAttack();
		return;
	}

	UpdateFollowMovement(DeltaTime);

	if (ActiveBeamActor)
	{
		ActiveBeamActor->SetActorLocation(GetFireOriginLocation());
		ActiveBeamActor->SetBeamEnd(GetDesiredBeamTargetLocation(), true);
	}
}

void AFunnel::StartBeamAttack(AActor* InTarget)
{
	SetBeamTarget(InTarget);

	if (!BeamActorClass || !IsValid(TargetActor) || AttackActiveDuration <= 0.0f)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(BeamFireTimerHandle);
	GetWorldTimerManager().ClearTimer(BeamDeactivateTimerHandle);
	GetWorldTimerManager().ClearTimer(AttackStopTimerHandle);

	bAttackActive = true;
	FireNextBeam();

	if (BeamFireInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			BeamFireTimerHandle,
			this,
			&AFunnel::FireNextBeam,
			BeamFireInterval,
			true);
	}

	GetWorldTimerManager().SetTimer(
		AttackStopTimerHandle,
		this,
		&AFunnel::StopBeamAttack,
		AttackActiveDuration,
		false);
}

void AFunnel::StopBeamAttack()
{
	GetWorldTimerManager().ClearTimer(BeamFireTimerHandle);
	GetWorldTimerManager().ClearTimer(BeamDeactivateTimerHandle);
	GetWorldTimerManager().ClearTimer(AttackStopTimerHandle);
	bAttackActive = false;
	DeactivateActiveBeam();

	if (!IsActorBeingDestroyed())
	{
		Destroy();
	}
}

void AFunnel::SetBeamTarget(AActor* InTarget)
{
	TargetActor = InTarget;
}

void AFunnel::UpdateFollowMovement(float DeltaTime)
{
	if (!IsValid(TargetActor))
	{
		return;
	}

	OrbitAngleDegrees = FMath::Fmod(OrbitAngleDegrees + RuntimeOrbitAngularSpeed * DeltaTime, 360.0f);

	const FVector OrbitCenter = GetDesiredBeamTargetLocation();
	const float OrbitRadians = FMath::DegreesToRadians(OrbitAngleDegrees);
	const FVector OrbitOffset(
		FMath::Cos(OrbitRadians) * RuntimeOrbitRadius,
		FMath::Sin(OrbitRadians) * RuntimeOrbitRadius,
		RuntimeOrbitHeight);

	const FVector FallbackDirection = GetActorLocation() - OrbitCenter;
	const FVector DistanceOffset = FallbackDirection.IsNearlyZero()
		? FVector(RuntimeDesiredDistanceToTarget, 0.0f, 0.0f)
		: FallbackDirection.GetSafeNormal() * RuntimeDesiredDistanceToTarget;

	const FVector DesiredLocation = OrbitCenter + OrbitOffset + DistanceOffset;
	const FVector NewLocation = FMath::VInterpTo(GetActorLocation(), DesiredLocation, DeltaTime, FollowSpeed);
	SetActorLocation(NewLocation);

	const FVector LookDirection = OrbitCenter - NewLocation;
	if (!LookDirection.IsNearlyZero())
	{
		SetActorRotation(LookDirection.Rotation());
	}
}

void AFunnel::FireNextBeam()
{
	if (!bAttackActive || !IsValid(TargetActor) || !BeamActorClass)
	{
		StopBeamAttack();
		return;
	}

	if (ActiveBeamActor)
	{
		ActiveBeamActor->Destroy();
		ActiveBeamActor = nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ActiveBeamActor = GetWorld()->SpawnActor<ABeamEffectActor>(
		BeamActorClass,
		GetFireOriginLocation(),
		GetActorRotation(),
		SpawnParams);

	if (ActiveBeamActor)
	{
		ActiveBeamActor->SetBeamEnd(GetDesiredBeamTargetLocation(), false);
		ActiveBeamActor->ApplyBeamParameters();
		ActiveBeamActor->ActivateBeam(true);

		GetWorldTimerManager().ClearTimer(BeamDeactivateTimerHandle);
		GetWorldTimerManager().SetTimer(
			BeamDeactivateTimerHandle,
			this,
			&AFunnel::DeactivateActiveBeam,
			BeamDuration,
			false);
	}
}

void AFunnel::DeactivateActiveBeam()
{
	if (ActiveBeamActor)
	{
		ActiveBeamActor->DeactivateBeam();
		ActiveBeamActor->Destroy();
		ActiveBeamActor = nullptr;
	}
}

FVector AFunnel::GetDesiredBeamTargetLocation() const
{
	return IsValid(TargetActor) ? TargetActor->GetActorLocation() + TargetOffset : FVector::ZeroVector;
}

FVector AFunnel::GetFireOriginLocation() const
{
	return FireOrigin ? FireOrigin->GetComponentLocation() : GetActorLocation();
}
