#include "SnakePathController.h"

USnakePathController::USnakePathController()
{
    PrimaryComponentTick.bCanEverTick = false; // 마스터에서 직접 호출하므로 틱은 꺼둠
    
    SourceOrbitRotation = FRotator::ZeroRotator;
    TargetOrbitRotation = FRotator::ZeroRotator;
}

void USnakePathController::UpdatePath(float DeltaTime, AActor* CenterActor, bool bPaused)
{
    if (!CenterActor) return;

    // 중심점 위치 초기화 (최초 실행 시)
    if (CurrentCenterLocation.IsZero())
    {
        CurrentCenterLocation = CenterActor->GetActorLocation();
    }

    // 일시 정지(RunPatten = true 등) 상태가 아닐 때만 진행
    if (!bPaused)
    {
        // 1. 주기적인 궤도 평면 변경 타이머
        PathChangeTimer += DeltaTime;
        if (PathChangeTimer >= PathChangeInterval)
        {
            PathChangeTimer = 0.0f;
            SourceOrbitRotation = TargetOrbitRotation;
            TargetOrbitRotation = FRotator(FMath::RandRange(-180.0f, 180.0f), FMath::RandRange(-180.0f, 180.0f), FMath::RandRange(-180.0f, 180.0f));
            bIsTransitioning = true;
            TransitionAlpha = 0.0f;
        }

        // 2. 궤도면 전환 보간
        if (bIsTransitioning)
        {
            TransitionAlpha += DeltaTime / TransitionDuration;
            if (TransitionAlpha >= 1.0f)
            {
                TransitionAlpha = 1.0f;
                bIsTransitioning = false;
            }
        }

        // 3. 중심점 이동 추적
        FVector TargetCenterLoc = CenterActor->GetActorLocation();
        CurrentCenterLocation = FMath::VInterpTo(CurrentCenterLocation, TargetCenterLoc, DeltaTime, OrbitInterpSpeed);
    }
}
