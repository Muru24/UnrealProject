// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBase.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "StatComponent.h"

ABulletBase::ABulletBase()
{
	PrimaryActorTick.bCanEverTick = true;

    CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionComp;

    CollisionComp->InitBoxExtent(FVector(10.0f, 5.0f, 5.0f));

    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));

    CollisionComp->OnComponentHit.AddDynamic(this, &ABulletBase::OnHit);
    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::OnOverlap);
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABulletBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    HandleLifeTime(DeltaTime);

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
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || OtherActor->IsA<ABulletBase>())
    {
        return;
    }

    if (UStatComponent* TargetStatComponent = OtherActor->FindComponentByClass<UStatComponent>())
    {
        TargetStatComponent->ApplyDamage(Damage);
    }

    SetActorLocation(ImpactLocation);
    Destroy();
}


