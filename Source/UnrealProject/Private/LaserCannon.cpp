// Fill out your copyright notice in the Description page of Project Settings.

#include "LaserCannon.h"
#include "BeamEffectActor.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ALaserCannon::ALaserCannon()
{
	PrimaryActorTick.bCanEverTick = true;

	FireOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("FireOrigin"));
	FireOrigin->SetupAttachment(RootComponent);

	if (!BeamActorClass)
	{
		BeamActorClass = ABeamEffectActor::StaticClass();
	}
}

void ALaserCannon::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStartBeamAttackOnBeginPlay && IsValid(TargetActor))
	{
		StartBeamAttack(TargetActor);
	}
}

void ALaserCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(TargetActor))
	{
		StopBeamAttack();
		return;
	}

	// 1. 현재 조준 지점 계산 (타겟 위치 + 연장 거리)
	const FVector AimLocation = GetTargetLocationWithExtension();

	// 2. 본체 회전 (조준 지점 지향)
	const FVector Direction = AimLocation - GetActorLocation();
	if (!Direction.IsNearlyZero())
	{
		SetActorRotation(Direction.Rotation());
	}

	// 3. 공격 중인 경우 빔 크기 업데이트
	if (bAttackActive && ActiveBeamActor)
	{
		// 경과 시간 업데이트
		AttackTimeTracker += DeltaTime;
		
		// 0.0 ~ 1.0 비율 계산
		const float Alpha = FMath::Clamp(AttackTimeTracker / AttackActiveDuration, 0.0f, 1.0f);
		
		// 빔 굵기 보간 (시작 크기 -> 끝 크기)
		const float CurrentSize = FMath::Lerp(InitialBeamSize, FinalBeamSize, Alpha);

		ActiveBeamActor->SetActorLocation(GetFireOriginLocation());
		ActiveBeamActor->SetBeamEnd(AimLocation, true);
		ActiveBeamActor->SetBeamSize(CurrentSize, true);
	}
}

void ALaserCannon::StartBeamAttack(AActor* InTarget)
{
	SetBeamTarget(InTarget);

	if (!BeamActorClass || !IsValid(TargetActor) || AttackActiveDuration <= 0.0f)
	{
		return;
	}

	// 기존 타이머 및 트래커 초기화
	GetWorldTimerManager().ClearTimer(BeamDeactivateTimerHandle);
	GetWorldTimerManager().ClearTimer(AttackStopTimerHandle);
	AttackTimeTracker = 0.0f;

	bAttackActive = true;
	
	FireBeam();

	// 공격 종료 타이머
	GetWorldTimerManager().SetTimer(
		AttackStopTimerHandle,
		this,
		&ALaserCannon::StopBeamAttack,
		AttackActiveDuration,
		false);
}

void ALaserCannon::StopBeamAttack()
{
	GetWorldTimerManager().ClearTimer(BeamDeactivateTimerHandle);
	GetWorldTimerManager().ClearTimer(AttackStopTimerHandle);
	
	bAttackActive = false;
	AttackTimeTracker = 0.0f;
	DeactivateActiveBeam();

	if (!IsActorBeingDestroyed())
	{
		Destroy();
	}
}

void ALaserCannon::SetBeamTarget(AActor* InTarget)
{
	TargetActor = InTarget;
}

void ALaserCannon::FireBeam()
{
	if (!bAttackActive || !IsValid(TargetActor) || !BeamActorClass)
	{
		return;
	}

	DeactivateActiveBeam();

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
		// 초기 빔 설정
		ActiveBeamActor->SetBeamEnd(GetTargetLocationWithExtension(), false);
		ActiveBeamActor->SetBeamSize(InitialBeamSize, false);
		ActiveBeamActor->ApplyBeamParameters();
		ActiveBeamActor->ActivateBeam(true);

		GetWorldTimerManager().SetTimer(
			BeamDeactivateTimerHandle,
			this,
			&ALaserCannon::DeactivateActiveBeam,
			BeamDuration,
			false);
	}
}

void ALaserCannon::DeactivateActiveBeam()
{
	if (ActiveBeamActor)
	{
		ActiveBeamActor->DeactivateBeam();
		ActiveBeamActor->Destroy();
		ActiveBeamActor = nullptr;
	}
}

FVector ALaserCannon::GetTargetLocationWithExtension() const
{
	if (!IsValid(TargetActor))
	{
		return FVector::ZeroVector;
	}

	const FVector FireOriginLoc = GetFireOriginLocation();
	const FVector TargetLoc = TargetActor->GetActorLocation();
	
	// 발사 지점에서 타겟까지의 방향
	const FVector Direction = (TargetLoc - FireOriginLoc).GetSafeNormal();

	// 타겟 지점을 기준으로 BeamExtraDistance만큼 더 연장된 좌표 반환
	return TargetLoc + (Direction * BeamExtraDistance);
}

FVector ALaserCannon::GetFireOriginLocation() const
{
	return FireOrigin ? FireOrigin->GetComponentLocation() : GetActorLocation();
}