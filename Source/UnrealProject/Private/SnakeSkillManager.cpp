#include "SnakeSkillManager.h"
#include "Snake_CompositeMaster.h"
#include "SnakeBodyChargeComponent.h"
#include "Components/ChildActorComponent.h"
#include "Kismet/GameplayStatics.h"

USnakeSkillManager::USnakeSkillManager()
{
    PrimaryComponentTick.bCanEverTick = false;
    SkillAutoTriggerTimer = 10.0f;
}

void USnakeSkillManager::Update(float DeltaTime, const TArray<UChildActorComponent*>& Segments)
{
    ASnake_CompositeMaster* Master = Cast<ASnake_CompositeMaster>(GetOwner());
    if (!Master) return;

    // 1. 자동 발동 타이머 (이동 중일 때만)
    if (!Master->RunPatten && !bIsSkillActive)
    {
        SkillAutoTriggerTimer -= DeltaTime;
        if (SkillAutoTriggerTimer <= 0.0f)
        {
            // 두 스킬 중 랜덤하게 발동
            if (FMath::RandBool())
            {
                StartBodyChargeSkill(Segments);
            }
            else
            {
                StartLaserSkill(Segments);
            }
        }
    }

    // 2. 스킬 활성 상태 관리
    if (bIsSkillActive)
    {
        bool bAllPartsIdle = true;
        bool bAllPartsFormed = true;

    // 모든 마디 상태 체크
    for (int32 i = 0; i < Segments.Num(); ++i)
    {
        if (Segments[i] && Segments[i]->GetChildActor())
        {
            if (USnakeBodyChargeComponent* SkillComp = Segments[i]->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
            {
                ESnakePartSkillState SkillState = SkillComp->GetCurrentState();
                if (SkillState != ESnakePartSkillState::Idle) bAllPartsIdle = false;
                
                // Forming 단계에서는 마디가 스스로 목표에 도착했는지 확인
                if (CurrentSkillPhase == ESnakeSkillState::Forming || CurrentSkillPhase == ESnakeSkillState::Laser_Preparing)
                {
                    if (!SkillComp->IsReachedFormation())
                    {
                        bAllPartsFormed = false;
                    }
                }
            }
        }
    }

        // 페이즈 전환 로직
        if (CurrentSkillPhase == ESnakeSkillState::Forming && bAllPartsFormed)
        {
            // 모든 마디에게 발사 신호
            for (UChildActorComponent* Part : Segments)
            {
                if (Part && Part->GetChildActor())
                {
                    if (USnakeBodyChargeComponent* SkillComp = Part->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
                    {
                        SkillComp->SignalLaunch();
                    }
                }
            }
            CurrentSkillPhase = ESnakeSkillState::Attacking;
        }
        else if (CurrentSkillPhase == ESnakeSkillState::Attacking && bAllPartsIdle)
        {
            // Charge 스킬 종료
            bIsSkillActive = false;
            CurrentSkillPhase = ESnakeSkillState::Idle;
            Master->RunPatten = false; // 이동 재개
            SkillAutoTriggerTimer = SkillAutoTriggerDelay;
        }
        else if (CurrentSkillPhase == ESnakeSkillState::Laser_Preparing)
        {
            // 모든 마디가 정렬될 때까지 대기
            bool bAllFormed = true;
            for (UChildActorComponent* Part : Segments)
            {
                if (Part && Part->GetChildActor())
                {
                    if (USnakeBodyChargeComponent* SkillComp = Part->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
                    {
                        if (SkillComp->GetCurrentState() != ESnakePartSkillState::Forming) continue;
                        
                        // 대략적인 위치 도달 체크
                        if (FVector::Dist(Part->GetChildActor()->GetActorLocation(), Part->GetComponentLocation()) > 100.0f)
                        {
                            // bAllFormed = false; // 일단은 시간 기반으로 넘어가거나 정교한 체크 가능
                        }
                    }
                }
            }

            StateTimer -= DeltaTime;
            if (StateTimer <= 0.0f)
            {
                CurrentSkillPhase = ESnakeSkillState::Laser_Firing;
                CurrentFiringIndex = 0;
                SequentialTimer = 0.0f;
                StateTimer = LaserDuration; // 전체 지속 시간
            }
        }
        else if (CurrentSkillPhase == ESnakeSkillState::Laser_Firing)
        {
            // 순차적 발사 처리
            if (CurrentFiringIndex < Segments.Num())
            {
                SequentialTimer -= DeltaTime;
                if (SequentialTimer <= 0.0f)
                {
                    if (Segments[CurrentFiringIndex] && Segments[CurrentFiringIndex]->GetChildActor())
                    {
                        if (USnakeBodyChargeComponent* SkillComp = Segments[CurrentFiringIndex]->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
                        {
                            SkillComp->FireLaser();
                        }
                    }
                    CurrentFiringIndex++;
                    SequentialTimer = LaunchDelayBetweenParts; // 0.2s
                }
            }

            // 전체 발사 지속 시간 관리
            StateTimer -= DeltaTime;
            if (StateTimer <= 0.0f && CurrentFiringIndex >= Segments.Num())
            {
                // 모든 마디 발사 중지
                for (UChildActorComponent* Part : Segments)
                {
                    if (Part && Part->GetChildActor())
                    {
                        if (USnakeBodyChargeComponent* SkillComp = Part->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
                        {
                            SkillComp->StopLaser();
                        }
                    }
                }
                
                bIsSkillActive = false;
                CurrentSkillPhase = ESnakeSkillState::Idle;
                Master->RunPatten = false;
                SkillAutoTriggerTimer = SkillAutoTriggerDelay;
            }
        }
    }
}

void USnakeSkillManager::StartBodyChargeSkill(const TArray<UChildActorComponent*>& Segments)
{
    ASnake_CompositeMaster* Master = Cast<ASnake_CompositeMaster>(GetOwner());
    if (!Master || Segments.Num() == 0 || bIsSkillActive) return;

    bIsSkillActive = true;
    Master->RunPatten = true; // 이동 멈춤
    CurrentSkillPhase = ESnakeSkillState::Forming;

    for (int32 i = 0; i < Segments.Num(); ++i)
    {
        if (Segments[i] && Segments[i]->GetChildActor())
        {
            if (USnakeBodyChargeComponent* SkillComp = Segments[i]->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
            {
                float AngleRad = FMath::DegreesToRadians((360.0f / FMath::Max(1, Segments.Num())) * i);
                FVector CircleOffset = FVector(FMath::Cos(AngleRad) * FormationRadius, FMath::Sin(AngleRad) * FormationRadius, 0.0f);
                FVector TargetFormPos = Master->GetActorLocation() + Master->GetActorRotation().RotateVector(CircleOffset);
                
                SkillComp->InitSkillSequence(UGameplayStatics::GetPlayerPawn(GetWorld(), 0), TargetFormPos, Master->GetActorRotation(), i * LaunchDelayBetweenParts);
            }
        }
    }
}

void USnakeSkillManager::StartLaserSkill(const TArray<UChildActorComponent*>& Segments)
{
    ASnake_CompositeMaster* Master = Cast<ASnake_CompositeMaster>(GetOwner());
    if (!Master || Segments.Num() == 0 || bIsSkillActive) return;

    bIsSkillActive = true;
    Master->RunPatten = true; 
    CurrentSkillPhase = ESnakeSkillState::Laser_Preparing;
    StateTimer = LaserPrepareTime;

    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    FVector BaseLoc = Master->GetActorLocation() + FVector(0, 0, LaserHeightOffset);
    FVector RightDir = Master->GetActorRightVector();

    for (int32 i = 0; i < Segments.Num(); ++i)
    {
        if (Segments[i] && Segments[i]->GetChildActor())
        {
            if (USnakeBodyChargeComponent* SkillComp = Segments[i]->GetChildActor()->FindComponentByClass<USnakeBodyChargeComponent>())
            {
                // 가로 일자 대형 위치 계산 (머리 기준 좌우로 정렬)
                float Offset = (i - (Segments.Num() / 2.0f)) * LaserLineSpacing;
                FVector TargetPos = BaseLoc + (RightDir * Offset);
                
                SkillComp->PrepareLaser(Player, TargetPos, Master->GetActorRotation());
            }
        }
    }
}
