// Fill out your copyright notice in the Description page of Project Settings.


#include "BeamObjcet.h"
#include "BeamEffectActor.h"
#include "LaserAttackComponent.h"

// Sets default values
ABeamObjcet::ABeamObjcet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LaserStart = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserStart"));
	SetRootComponent(LaserStart);

	LaserEnd = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserEnd"));
	LaserEnd->SetupAttachment(LaserStart);

	LaserEffect = CreateDefaultSubobject<ULaserAttackComponent>(TEXT("LaserEffect"));

	FireOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("FireOrigin"));
	FireOrigin->SetupAttachment(LaserStart);

	FireEnd = CreateDefaultSubobject<USceneComponent>(TEXT("FireEnd"));
	FireEnd->SetupAttachment(LaserEnd);
}

void ABeamObjcet::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (LaserEnd)
	{
		LaserEnd->SetRelativeLocation(FVector(BeamDistance, 0.0f, 0.0f));
	}
}

// Called when the game starts or when spawned
void ABeamObjcet::BeginPlay()
{
	Super::BeginPlay();
	LaserEffect->SetFireOrigin(FireOrigin);
	LaserEffect->SetBeamExtraDistance(0.0f);
	LaserEffect->StartLaserFromComponent(FireEnd);
}


// Called every frame
void ABeamObjcet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

