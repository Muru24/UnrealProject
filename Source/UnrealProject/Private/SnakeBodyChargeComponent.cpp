#include "SnakeBodyChargeComponent.h"
#include "GameFramework/Actor.h"
#include "Snake_CompositeMaster.h"
#include "Components/ChildActorComponent.h"
#include "LaserAttackComponent.h"

USnakeBodyChargeComponent::USnakeBodyChargeComponent()
{
    PrimaryComponentTick.bCanEverTick = true; 
    LaserComponent = CreateDefaultSubobject<ULaserAttackComponent>(TEXT("LaserComponent"));
}

void USnakeBodyChargeComponent::BeginPlay()
{
    Super::BeginPlay();
}

void USnakeBodyChargeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    AActor* Owner = GetOwner();
    if (Owner)
    {
        // 자신이 ChildActorComponent를 통해 생성된 경우 마스터에게 자신을 해제하도록 요청
        if (UChildActorComponent* ParentComp = Cast<UChildActorComponent>(Owner->GetParentComponent()))
        {
            if (ASnake_CompositeMaster* Master = Cast<ASnake_CompositeMaster>(ParentComp->GetOwner()))
            {
                Master->UnregisterSegment(ParentComp);
            }
        }
    }
    Super::EndPlay(EndPlayReason);
}

void USnakeBodyChargeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner) return;

    FVector CurrentLoc = Owner->GetActorLocation();
    FRotator CurrentRot = Owner->GetActorRotation();

    // 현재 궤도 기준 나선형(Slither) 목표 위치 계산
    FVector SlitherTargetLoc = GetSlitherLocation(TargetBaseLocation, OrbitOutward, OrbitUp, OrbitAngle);

    switch (CurrentState)
    {
        case ESnakePartSkillState::Idle:
        {
            // 목표 위치(나선형 이동 포함)로 부드럽게 보간 이동
            FVector NewLoc = FMath::VInterpTo(CurrentLoc, SlitherTargetLoc, DeltaTime, 15.0f);
            FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetBaseRotation, DeltaTime, 15.0f);

            Owner->SetActorLocation(NewLoc);
            Owner->SetActorRotation(NewRot);
            break;
        }

        case ESnakePartSkillState::Forming:
        {
            // 1. 대형 형성 위치로 이동
            // 플레이어 실시간 추적 (회전)
            if (TargetActor)
            {
                FormationRotation = (TargetActor->GetActorLocation() - CurrentLoc).Rotation();
            }

            FVector NewLoc = FMath::VInterpTo(CurrentLoc, FormationLocation, DeltaTime, ReturnSpeed / 100.0f);
            FRotator NewRot = FMath::RInterpTo(CurrentRot, FormationRotation, DeltaTime, ReturnSpeed / 100.0f);

            Owner->SetActorLocation(NewLoc);
            Owner->SetActorRotation(NewRot);

            // 대형 형성 위치 도착 판정
            if (FVector::Dist(NewLoc, FormationLocation) < 50.0f)
            {
                bReachedFormation = true;
            }
            else
            {
                bReachedFormation = false;
            }

            // 2. 관리자가 발사 신호를 보냈다면 지연 시간 카운트다운
            if (bLaunchSignaled)
            {
                SequentialLaunchDelay -= DeltaTime;
                if (SequentialLaunchDelay <= 0.0f)
                {
                    ActivateSkill(TargetActor);
                }
            }
            break;
        }

        case ESnakePartSkillState::Flying:
        {
            // 돌진 공격
            StateTimer -= DeltaTime;
            // 발사 당시의 위치가 아닌 실시간 타겟 위치 추적 가능 (기존 로직 유지)
            FVector TargetLoc = TargetActor ? TargetActor->GetActorLocation() : FlightTargetLocation;

            FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaTime, FlySpeed / 100.0f);
            Owner->SetActorLocation(NewLoc);

            if (StateTimer <= 0.0f || FVector::Dist(CurrentLoc, TargetLoc) < 50.0f)
            {
                CurrentState = ESnakePartSkillState::Returning;
            }
            break;
        }

        case ESnakePartSkillState::Returning:
        {
            // 복귀: 나선형 위치로 부드러운 보간 이동
            FVector NewLoc = FMath::VInterpTo(CurrentLoc, SlitherTargetLoc, DeltaTime, ReturnSpeed / 100.0f);
            FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetBaseRotation, DeltaTime, ReturnSpeed / 100.0f);

            Owner->SetActorLocation(NewLoc);
            Owner->SetActorRotation(NewRot);

            if (FVector::Dist(CurrentLoc, SlitherTargetLoc) < 50.0f)
            {
                CurrentState = ESnakePartSkillState::Idle;
            }
            break;
        }

        case ESnakePartSkillState::LaserFiring:
        {
            // 레이저 발사 중에는 LaserComponent가 조준 및 회전을 처리함
            // 여기서는 추가적인 마디별 로직(예: 반동 연출 등)이 필요하면 작성
            break;
        }
    }
}

void USnakeBodyChargeComponent::ActivateSkill(AActor* InTarget)
{
    if (!InTarget) return;

    TargetActor = InTarget;
    FlightTargetLocation = InTarget->GetActorLocation();
    CurrentState = ESnakePartSkillState::Flying;
    StateTimer = SkillDuration;
    bLaunchSignaled = false; // 플래그 초기화
}

void USnakeBodyChargeComponent::InitSkillSequence(AActor* InTarget, const FVector& InFormLoc, const FRotator& InFormRot, float InLaunchDelay)
{
    TargetActor = InTarget;
    FormationLocation = InFormLoc;
    FormationRotation = InFormRot;
    SequentialLaunchDelay = InLaunchDelay;
    
    CurrentState = ESnakePartSkillState::Forming;
    bLaunchSignaled = false;
    bReachedFormation = false;
}

void USnakeBodyChargeComponent::PrepareLaser(AActor* InTarget, const FVector& InFormLoc, const FRotator& InFormRot)
{
    TargetActor = InTarget;
    FormationLocation = InFormLoc;
    FormationRotation = InFormRot;
    CurrentState = ESnakePartSkillState::Forming;
    bLaunchSignaled = false;
    bReachedFormation = false;
}

void USnakeBodyChargeComponent::FireLaser()
{
    if (LaserComponent && TargetActor)
    {
        LaserComponent->StartLaser(TargetActor);
    }
}

void USnakeBodyChargeComponent::StopLaser()
{
    if (LaserComponent)
    {
        LaserComponent->StopLaser();
    }
    CurrentState = ESnakePartSkillState::Returning;
}

void USnakeBodyChargeComponent::UpdateTargetInfo(const FVector& InBaseLoc, const FVector& InOutward, const FVector& InUp, float InAngle, const FRotator& InRot)
{
    TargetBaseLocation = InBaseLoc;
    OrbitOutward = InOutward;
    OrbitUp = InUp;
    OrbitAngle = InAngle;
    TargetBaseRotation = InRot;
}

FVector USnakeBodyChargeComponent::GetSlitherLocation(const FVector& BaseLocation, const FVector& Outward, const FVector& Up, float Angle)
{
    float TimeComponent = GetWorld()->GetTimeSeconds() * CrawlSpeed;
    float Phase = (Angle * SpiralSpeed * 10.0f) + TimeComponent;

    FVector Offset = (Outward * FMath::Cos(Phase) * SlitherRadius) +
                     (Up * FMath::Sin(Phase) * SlitherRadius);

    return BaseLocation + Offset;
}
