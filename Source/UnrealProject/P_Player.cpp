// Fill out your copyright notice in the Description page of Project Settings.


#include "P_Player.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "LockOnComponent.h"
#include "BulletBase.h"
#include "BulletWeaponComponent.h"
#include "PathFollowerComponent.h"

AP_Player::AP_Player()
{
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 500.0f;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 3.0f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    LockOn = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOn"));

    BWeaponComp = CreateDefaultSubobject<UBulletWeaponComponent>(TEXT("B_Weapon"));

    PathFollower = CreateDefaultSubobject<UPathFollowerComponent>(TEXT("PathFollowerComp"));
}

void AP_Player::BeginPlay()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        PC->bShowMouseCursor = true; 
        PC->SetInputMode(FInputModeGameAndUI());
    }

    APawn_Template::BeginPlay();
}

void AP_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &APawn_Template::Fire);
    PlayerInputComponent->BindAction(TEXT("LockOn"), IE_Pressed, LockOn,&ULockOnComponent::TraceTarget);
    PlayerInputComponent->BindAction(TEXT("TargetChange"), IE_Pressed, LockOn,&ULockOnComponent::ChangeTarget);
}

void AP_Player::Fire()
{

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC || !BWeaponComp || !LockOn) return;

    FVector TargetPoint;
    AActor* CurrentTarget = nullptr;

    if (LockOn->isLockOn && LockOn->target)
    {
        TargetPoint = LockOn->target->GetActorLocation();
        CurrentTarget = LockOn->target;
    }
    else
    {
        FVector MouseLocation, MouseDirection;
        if (PC->DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
        {
            FVector Start = MouseLocation;
            FVector End = Start + (MouseDirection * 10000.f);

            FHitResult HitResult;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(this);

            bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

            if (bHit)
            {
                TargetPoint = HitResult.Location;

                // 디버그 드로우 
                DrawDebugSphere(GetWorld(), HitResult.Location, 20.f, 12, FColor::Red, false, 2.0f);
                if (HitResult.GetActor())
                {
                    DrawDebugString(GetWorld(), HitResult.Location + FVector(0, 0, 50), HitResult.GetActor()->GetName(), nullptr, FColor::White, 2.0f);
                }
            }
            else
            {
                TargetPoint = End;
            }
        }
    }

    BWeaponComp->Fire(TargetPoint, CurrentTarget);
}


