// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBase.h"
#include "BossCore.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "StatComponent.h"

ABulletBase::ABulletBase()
{
	PrimaryActorTick.bCanEverTick = true;

    CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionComp;

    CollisionComp->InitBoxExtent(FVector(10.0f, 5.0f, 5.0f));

    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComp->SetGenerateOverlapEvents(true);

    CollisionComp->OnComponentHit.AddDynamic(this, &ABulletBase::OnHit);
    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::OnOverlap);

    BulletVisualEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletVisual"));
    BulletVisualEffect->SetupAttachment(RootComponent);
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();

    CurrentHitPoints = FMath::Max(0.0f, MaxHitPoints);
    CurrentMoveSpeed = FMath::Max(0.0f, Speed);
    SpeedRampElapsedTime = 0.0f;
	
    if (BulletVisualEffect && SelectedEffect)
    {
        BulletVisualEffect->SetAsset(SelectedEffect);
        BulletVisualEffect->Activate();
    }
}

void ABulletBase::ConfigureAttackType(EBulletAttackType InAttackType, int32 InRemainingPenetrations, float InExplosionRadius)
{
    AttackType = InAttackType;
    RemainingPenetrations = FMath::Max(0, InRemainingPenetrations);
    ExplosionRadius = FMath::Max(0.0f, InExplosionRadius);
}

void ABulletBase::SetProjectileSpeed(float InSpeed, float InMaxSpeed)
{
    Speed = FMath::Max(0.0f, InSpeed);
    MaxSpeed = FMath::Max(Speed, InMaxSpeed);
    CurrentMoveSpeed = Speed;
}

void ABulletBase::ConfigureSpeedRamp(float InSpeedRampDelay, float InSpeedRampInterpSpeed)
{
    SpeedRampDelay = FMath::Max(0.0f, InSpeedRampDelay);
    SpeedRampInterpSpeed = FMath::Max(0.0f, InSpeedRampInterpSpeed);
    SpeedRampElapsedTime = 0.0f;
}

void ABulletBase::ConfigureShotDown(float InMaxHitPoints, bool bInCanBeShotDown)
{
    bCanBeShotDown = bInCanBeShotDown;
    MaxHitPoints = FMath::Max(0.0f, InMaxHitPoints);
    CurrentHitPoints = MaxHitPoints;
}

void ABulletBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    HandleLifeTime(DeltaTime);

    if (CurrentMoveSpeed < MaxSpeed && SpeedRampInterpSpeed > 0.0f)
    {
        SpeedRampElapsedTime += DeltaTime;
        if (SpeedRampElapsedTime >= SpeedRampDelay)
        {
            CurrentMoveSpeed = FMath::FInterpTo(CurrentMoveSpeed, MaxSpeed, DeltaTime, SpeedRampInterpSpeed);
        }
    }

}

void ABulletBase::HandleLifeTime(float DeltaTime)
{
    LifeTime -= DeltaTime;
    if (LifeTime <= 0.0f)
    {
        Destroy();
    }
}

void ABulletBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ProcessImpact(OtherActor, SweepResult.Location);
}

void ABulletBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    ProcessImpact(OtherActor, Hit.Location);
}

void ABulletBase::ProcessImpact(AActor* OtherActor, const FVector& ImpactLocation)
{
    if (!CanAffectActor(OtherActor))
    {
        return;
    }

    if (DamagedActors.Contains(OtherActor))
    {
        return;
    }

    switch (AttackType)
    {
    case EBulletAttackType::Piercing:
        if (!ApplyDamageToActor(OtherActor))
        {
            return;
        }
        if (CollisionComp)
        {
            CollisionComp->IgnoreActorWhenMoving(OtherActor, true);
        }
        --RemainingPenetrations;
        if (RemainingPenetrations < 0)
        {
            if (HitEffect)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, ImpactLocation);
            }
            Destroy();
        }
        break;

    case EBulletAttackType::Explosive:
        ApplyExplosionDamage(ImpactLocation, OtherActor);
        if (HitEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, ImpactLocation);
        }
        Destroy();
        break;

    case EBulletAttackType::NonPiercing:
    default:
        if (!ApplyDamageToActor(OtherActor))
        {
            return;
        }
        if (HitEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, ImpactLocation);
        }
        Destroy();
        break;
    }
}

bool ABulletBase::ApplyDamageToActor(AActor* TargetActorToDamage)
{
    if (!TargetActorToDamage || DamagedActors.Contains(TargetActorToDamage))
    {
        return false;
    }

    if (const ABossCore* BossCore = Cast<ABossCore>(TargetActorToDamage))
    {
        if (!BossCore->CanReceiveCoreDamage())
        {
            return false;
        }
    }

    if (ABulletBase* TargetBullet = Cast<ABulletBase>(TargetActorToDamage))
    {
        if (!TargetBullet->CanBeShotDown())
        {
            return false;
        }

        if (TargetBullet->ApplyBulletDamage(Damage))
        {
            DamagedActors.Add(TargetActorToDamage);
            return true;
        }
    }

    if (UStatComponent* TargetStatComponent = TargetActorToDamage->FindComponentByClass<UStatComponent>())
    {
        TargetStatComponent->ApplyDamage(Damage);
        DamagedActors.Add(TargetActorToDamage);
        return true;
    }

    return false;
}

bool ABulletBase::ApplyBulletDamage(float DamageAmount)
{
    if (!bCanBeShotDown || DamageAmount <= 0.0f)
    {
        return false;
    }

    CurrentHitPoints = FMath::Clamp(CurrentHitPoints - DamageAmount, 0.0f, MaxHitPoints);
    if (CurrentHitPoints <= 0.0f)
    {
        if (HitEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, GetActorLocation());
        }
        Destroy();
    }

    return true;
}

void ABulletBase::ApplyExplosionDamage(const FVector& ImpactLocation, AActor* DirectHitActor)
{
    ApplyDamageToActor(DirectHitActor);

    if (!GetWorld() || ExplosionRadius <= 0.0f)
    {
        return;
    }

    TArray<FOverlapResult> OverlapResults;
    FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams::AllDynamicObjects;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(GetOwner());

    const bool bHasOverlap = GetWorld()->OverlapMultiByObjectType(
        OverlapResults,
        ImpactLocation,
        FQuat::Identity,
        ObjectQueryParams,
        FCollisionShape::MakeSphere(ExplosionRadius),
        QueryParams);

    if (!bHasOverlap)
    {
        return;
    }

    for (const FOverlapResult& OverlapResult : OverlapResults)
    {
        AActor* OverlapActor = OverlapResult.GetActor();
        if (!CanAffectActor(OverlapActor))
        {
            continue;
        }

        ApplyDamageToActor(OverlapActor);
    }
}

bool ABulletBase::CanAffectActor(AActor* OtherActor) const
{
    if (!OtherActor || OtherActor == this)
    {
        return false;
    }

    if (const ABulletBase* OtherBullet = Cast<ABulletBase>(OtherActor))
    {
        if (!OtherBullet->CanBeShotDown())
        {
            return false;
        }
    }

    AActor* MyOwner = GetOwner();
    if (OtherActor == MyOwner)
    {
        return false;
    }

    // 내 주인의 주인도 무시 (예: 기체의 주인인 플레이어 본체)
    if (MyOwner && OtherActor == MyOwner->GetOwner())
    {
        return false;
    }

    return true;
}


