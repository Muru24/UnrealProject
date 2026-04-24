// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBase.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "StatComponent.h"
#include "NiagaraComponent.h"

ABulletBase::ABulletBase()
{
	PrimaryActorTick.bCanEverTick = true;

    CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionComp;

    CollisionComp->InitBoxExtent(FVector(10.0f, 5.0f, 5.0f));

    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));

    CollisionComp->OnComponentHit.AddDynamic(this, &ABulletBase::OnHit);
    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::OnOverlap);

    BulletVisualEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletVisual"));
    BulletVisualEffect->SetupAttachment(RootComponent);
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	
    if (BulletVisualEffect && SelectedEffect)
    {
        BulletVisualEffect->SetAsset(SelectedEffect);
        BulletVisualEffect->Activate();
    }
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
    UE_LOG(LogTemp, Warning, TEXT("overlap!"));
}

void ABulletBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    ProcessImpact(OtherActor, Hit.Location);
    UE_LOG(LogTemp, Warning, TEXT("hit!"));
}

void ABulletBase::ProcessImpact(AActor* OtherActor, const FVector& ImpactLocation)
{
   /* if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || OtherActor->IsA<ABulletBase>())
    {
        return;
    }*/

    //if (UStatComponent* TargetStatComponent = OtherActor->FindComponentByClass<UStatComponent>())
    //{
    //    //실제 게임일때는 damge
    //    TargetStatComponent->ApplyDamage(0);
    //}


    //SetActorLocation(ImpactLocation);
    //Destroy();
}


