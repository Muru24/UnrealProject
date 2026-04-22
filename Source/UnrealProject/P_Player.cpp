// Fill out your copyright notice in the Description page of Project Settings.


#include "P_Player.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "LockOnComponent.h"
#include "BulletBase.h"

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
    if (!PC || !BulletBase || !LockOn) return;

    FVector MouseLocation, MouseDirection;
    FVector TargetPoint;

    // 디버그용 변수들
    FHitResult HitResult;
    bool bHit = false;

    if (!LockOn->isLockOn)
    {
        if (!PC->DeprojectMousePositionToWorld(MouseLocation, MouseDirection)) return;

        FVector Start = MouseLocation;
        FVector End = Start + (MouseDirection * 10000.f);

        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this); 

        bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

        if (bHit)
        {
            TargetPoint = HitResult.Location;

            // --- 디버그 드로우 추가 ---
            // 충돌 지점에 구체 그리기
            DrawDebugSphere(GetWorld(), HitResult.Location, 20.f, 12, FColor::Red, false, 2.0f);
            // 충돌한 액터 이름 표시
            if (HitResult.GetActor())
            {
                FString ActorName = HitResult.GetActor()->GetName();
                DrawDebugString(GetWorld(), HitResult.Location + FVector(0, 0, 50), ActorName, nullptr, FColor::White, 2.0f);
            }
        }
        else
        {
            TargetPoint = End;
        }
    }
    else
    {
        if (LockOn->target)
        {
            TargetPoint = LockOn->target->GetActorLocation();
        }
        else return;
    }

    for (USceneComponent* Point : FirePoints)
    {
        if (!Point) continue;

        FVector SpawnLocation = Point->GetComponentLocation();
        FVector LookAtDir = (TargetPoint - SpawnLocation).GetSafeNormal();
        FRotator LookAtRotation = LookAtDir.Rotation();

        Point->SetWorldRotation(LookAtRotation);

        ABulletBase* NewBullet = GetWorld()->SpawnActor<ABulletBase>(BulletBase, SpawnLocation, LookAtRotation);        
        if (NewBullet)
        {
            if (LockOn->isLockOn && LockOn->target)
            {
                NewBullet->SetTarget(LockOn->target);
            }
        }
    }
}


