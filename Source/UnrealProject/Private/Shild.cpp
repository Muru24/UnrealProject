// Fill out your copyright notice in the Description page of Project Settings.


#include "Shild.h"
#include "StatComponent.h"
// Sets default values
AShild::AShild()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	State = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
}

// Called when the game starts or when spawned
void AShild::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShild::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

