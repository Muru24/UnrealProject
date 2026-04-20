// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_Template.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"

APawn_Template::APawn_Template()
{
	PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;
    CollisionComponent->SetSphereRadius(100.f);
    CollisionComponent->SetCollisionProfileName(TEXT("Pawn"));

    PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
    PlaneMesh->SetupAttachment(RootComponent);
    PlaneMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 500.0f;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 3.0f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;
}

void APawn_Template::BeginPlay()
{
	Super::BeginPlay();
	
}

void APawn_Template::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    Move_Pawn(DeltaTime);
}

void APawn_Template::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APawn_Template::Initialize_GameManager_Pawn()
{

}

void APawn_Template::Move_Pawn(float DeltaTime)
{
    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan,
            FString::Printf(TEXT("Current MaxSpeed: %f"), MaxSpeed));
    }
    FVector ForwardMovement = GetActorForwardVector() * MaxSpeed * DeltaTime;
	AddActorLocalOffset(ForwardMovement, true);
}


