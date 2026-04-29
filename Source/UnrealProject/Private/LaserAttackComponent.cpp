#include "LaserAttackComponent.h"
#include "BeamEffectActor.h"
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

    if (bAttackActive && ActiveBeamActor)
    {
        if (!IsValid(TargetActor))
        {
            StopLaser();
            return;
        }

        // 경과 시간 업데이트
        AttackTimeTracker += DeltaTime;
        
        // 0.0 ~ 1.0 비율 계산
        const float Alpha = FMath::Clamp(AttackTimeTracker / AttackActiveDuration, 0.0f, 1.0f);
        
        // 빔 굵기 보간
        const float CurrentSize = FMath::Lerp(InitialBeamSize, FinalBeamSize, Alpha);

        FVector AimLocation = GetTargetLocationWithExtension();
        ActiveBeamActor->SetActorLocation(GetFireOriginLocation());
        ActiveBeamActor->SetBeamEnd(AimLocation, true);
        ActiveBeamActor->SetBeamSize(CurrentSize, true);

        // 본체 회전 (주인이 액터일 경우 조준 지점 바라보기)
        AActor* Owner = GetOwner();
        if (Owner)
        {
            FVector Direction = AimLocation - Owner->GetActorLocation();
            if (!Direction.IsNearlyZero())
            {
                Owner->SetActorRotation(Direction.Rotation());
            }
        }
    }
}

void ULaserAttackComponent::StartLaser(AActor* InTarget)
{
    if (!InTarget) return;

    TargetActor = InTarget;
    AttackTimeTracker = 0.0f;
    bAttackActive = true;

    FireBeam();

    // 공격 종료 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(
        AttackStopTimerHandle,
        this,
        &ULaserAttackComponent::StopLaser,
        AttackActiveDuration,
        false);
}

void ULaserAttackComponent::StopLaser()
{
    GetWorld()->GetTimerManager().ClearTimer(AttackStopTimerHandle);
    bAttackActive = false;
    DeactivateActiveBeam();
}

void ULaserAttackComponent::FireBeam()
{
    if (!bAttackActive || !IsValid(TargetActor) || !BeamActorClass) return;

    DeactivateActiveBeam();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    SpawnParams.Instigator = Cast<APawn>(GetOwner());
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ActiveBeamActor = GetWorld()->SpawnActor<ABeamEffectActor>(
        BeamActorClass,
        GetFireOriginLocation(),
        GetOwner()->GetActorRotation(),
        SpawnParams);

    if (ActiveBeamActor)
    {
        ActiveBeamActor->SetBeamEnd(GetTargetLocationWithExtension(), false);
        ActiveBeamActor->SetBeamSize(InitialBeamSize, false);
        ActiveBeamActor->ApplyBeamParameters();
        ActiveBeamActor->ActivateBeam(true);
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

FVector ULaserAttackComponent::GetTargetLocationWithExtension() const
{
    if (!IsValid(TargetActor)) return FVector::ZeroVector;

    const FVector Origin = GetFireOriginLocation();
    const FVector TargetLoc = TargetActor->GetActorLocation();
    const FVector Direction = (TargetLoc - Origin).GetSafeNormal();

    return TargetLoc + (Direction * BeamExtraDistance);
}

FVector ULaserAttackComponent::GetFireOriginLocation() const
{
    return FireOrigin ? FireOrigin->GetComponentLocation() : GetOwner()->GetActorLocation();
}
