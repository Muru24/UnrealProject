// Fill out your copyright notice in the Description page of Project Settings.


#include "Snake_CompositeMaster.h"
#include "Kismet/GameplayStatics.h"
#include "SnakeBodyChargeComponent.h"
#include "BossPhaseComponent.h"
#include "SnakePathController.h"
#include "SnakeSkillManager.h"

ASnake_CompositeMaster::ASnake_CompositeMaster()
{
	PrimaryActorTick.bCanEverTick = true;
    
    PathController = CreateDefaultSubobject<USnakePathController>(TEXT("PathController"));
    HeadSkillComp = CreateDefaultSubobject<USnakeBodyChargeComponent>(TEXT("HeadSkillComp"));
    SkillManager = CreateDefaultSubobject<USnakeSkillManager>(TEXT("SkillManager"));
}

void ASnake_CompositeMaster::BeginPlay()
{
    Super::BeginPlay();
    
    CurrentOrbitAngle = 0.0f;
    ShrinkSpeed = 3.0f;
    ApplyBossPhase(EBossEncounterPhase::Intro);
}

void ASnake_CompositeMaster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!CenterActor) return;

    // 1. 스킬 시스템 업데이트 (SkillManager에게 위임)
    SkillManager->Update(DeltaTime, ChildParts);

    // 2. 궤도 경로 업데이트 (PathController에게 위임)
    PathController->UpdatePath(DeltaTime, CenterActor, RunPatten);

    if (!RunPatten)
    {
        CurrentOrbitAngle += OrbitSpeed * DeltaTime;
    }

    // 컨트롤러로부터 현재 경로 정보 획득
    FVector CurrentCenterLocation = PathController->GetCurrentCenterLocation();
    FRotator SourceOrbitRotation = PathController->GetSourceOrbitRotation();
    FRotator TargetOrbitRotation = PathController->GetTargetOrbitRotation();
    float TransitionAlpha = PathController->GetTransitionAlpha();

    // 3. 마디 오프셋 동기화 (추가된 마디가 있을 경우 대비)
    while (SegAngleOffsets.Num() < ChildParts.Num())
    {
        SegAngleOffsets.Add((SegAngleOffsets.Num() + 1) * SegmentAngleGap);
    }

    // 4. 머리 및 마디 목표 위치 계산
    FVector OrbitNormal = CenterActor->GetActorUpVector(); 

    // 머리(Master) 위치 업데이트
    {
        FVector PosA, FwdA, RtA, UpA, PosB, FwdB, RtB, UpB;
        GetOrbitTransform(CurrentOrbitAngle, SourceOrbitRotation, PosA, FwdA, RtA, UpA, CurrentCenterLocation);
        GetOrbitTransform(CurrentOrbitAngle, TargetOrbitRotation, PosB, FwdB, RtB, UpB, CurrentCenterLocation);
        
        FVector BlendedLoc = FMath::Lerp(PosA, PosB, TransitionAlpha);
        FVector Outward = FMath::Lerp((PosA - CurrentCenterLocation).GetSafeNormal(), (PosB - CurrentCenterLocation).GetSafeNormal(), TransitionAlpha).GetSafeNormal();
        FVector Tangent = FMath::Lerp((-FwdA * FMath::Sin(CurrentOrbitAngle) + RtA * FMath::Cos(CurrentOrbitAngle)), (-FwdB * FMath::Sin(CurrentOrbitAngle) + RtB * FMath::Cos(CurrentOrbitAngle)), TransitionAlpha);
        
        // 머리 전용 컴포넌트에게 궤도 정보 전달 (나선형 이동은 컴포넌트가 처리)
        HeadSkillComp->UpdateTargetInfo(BlendedLoc, Outward, OrbitNormal, CurrentOrbitAngle, Tangent.Rotation());
    }

    bool bAllPartsIdle = true;
    bool bAllPartsFormed = true;

    // 마디(ChildParts) 목표값 전달
    for (int32 i = 0; i < ChildParts.Num(); ++i)
    {
        if (ChildParts[i] && ChildParts[i]->GetChildActor())
        {
            float TargetOffset = (i + 1) * SegmentAngleGap;
            SegAngleOffsets[i] = FMath::FInterpTo(SegAngleOffsets[i], TargetOffset, DeltaTime, ShrinkSpeed);
            float SegAngle = CurrentOrbitAngle - SegAngleOffsets[i];
            
            FVector PosA, FwdA, RtA, UpA, PosB, FwdB, RtB, UpB;
            GetOrbitTransform(SegAngle, SourceOrbitRotation, PosA, FwdA, RtA, UpA, CurrentCenterLocation);
            GetOrbitTransform(SegAngle, TargetOrbitRotation, PosB, FwdB, RtB, UpB, CurrentCenterLocation);
            FVector BlendedLoc = FMath::Lerp(PosA, PosB, TransitionAlpha);
            FVector Outward = FMath::Lerp((PosA - CurrentCenterLocation).GetSafeNormal(), (PosB - CurrentCenterLocation).GetSafeNormal(), TransitionAlpha).GetSafeNormal();
            FVector Tangent = FMath::Lerp((-FwdA * FMath::Sin(SegAngle) + RtA * FMath::Cos(SegAngle)), (-FwdB * FMath::Sin(SegAngle) + RtB * FMath::Cos(SegAngle)), TransitionAlpha);
            
            if (USnakeBodyChargeComponent* SkillComp = ChildParts[i]->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
            {
                ESnakePartSkillState SkillState = SkillComp->GetCurrentState();
                FVector TargetFormPos = BlendedLoc;
                FRotator TargetFormRot = Tangent.Rotation();

                if (SkillManager->IsSkillActive())
                {
                    // 스킬 중일 때의 목표 위치 계산 로직 (간소화 가능)
                    // SkillManager 내부 로직과 동기화 필요 시 여기서 호출하거나 위임
                }

                // 컴포넌트에게 궤도 정보 전달
                SkillComp->UpdateTargetInfo(BlendedLoc, Outward, OrbitNormal, SegAngle, Tangent.Rotation()); 
            }
        }
    }
}

void ASnake_CompositeMaster::ApplyBossPhase(EBossEncounterPhase NewPhase)
{
    if (SkillManager)
    {
        SkillManager->ApplyBossPhase(NewPhase);
    }

    switch (NewPhase)
    {
    case EBossEncounterPhase::Intro:
        OrbitSpeed = 0.6f;
        ShrinkSpeed = 2.5f;
        break;
    case EBossEncounterPhase::Phase1:
        OrbitSpeed = 0.9f;
        ShrinkSpeed = 3.0f;
        break;
    case EBossEncounterPhase::Phase2:
        OrbitSpeed = 1.15f;
        ShrinkSpeed = 3.75f;
        break;
    case EBossEncounterPhase::Enraged:
        OrbitSpeed = 1.4f;
        ShrinkSpeed = 4.5f;
        break;
    case EBossEncounterPhase::Defeated:
        RunPatten = false;
        break;
    default:
        break;
    }
}

void ASnake_CompositeMaster::UnregisterSegment(UChildActorComponent* SegmentComp)
{
    int32 Index = ChildParts.Find(SegmentComp);
    if (Index != INDEX_NONE)
    {
        ChildParts.RemoveAt(Index);
        if (SegAngleOffsets.IsValidIndex(Index))
        {
            SegAngleOffsets.RemoveAt(Index);
        }
    }
}

void ASnake_CompositeMaster::GetOrbitTransform(float Angle, const FRotator& Rotation, FVector& OutLocation, FVector& OutForward, FVector& OutRight, FVector& OutUp, const FVector& CenterLoc)
{
    FQuat OrbitQuat(Rotation);
    OutForward = OrbitQuat.GetForwardVector();
    OutRight = OrbitQuat.GetRightVector();
    OutUp = OrbitQuat.GetUpVector();

    FVector Outward = (OutForward * FMath::Cos(Angle) + OutRight * FMath::Sin(Angle));
    OutLocation = CenterLoc + Outward * OrbitRadius;
}

