#include "SnakePathController.h"

USnakePathController::USnakePathController()
{
    PrimaryComponentTick.bCanEverTick = false; 
    
    SourceOrbitRotation = FRotator::ZeroRotator;
    TargetOrbitRotation = FRotator::ZeroRotator;
}

void USnakePathController::UpdatePath(float DeltaTime, AActor* CenterActor, bool bPaused)
{
    if (!CenterActor) return;

    if (CurrentCenterLocation.IsZero())
    {
        CurrentCenterLocation = CenterActor->GetActorLocation();
    }

    if (!bPaused)
    {
        PathChangeTimer += DeltaTime;
        if (PathChangeTimer >= PathChangeInterval)
        {
            PathChangeTimer = 0.0f;
            SourceOrbitRotation = TargetOrbitRotation;
            TargetOrbitRotation = FRotator(FMath::RandRange(-180.0f, 180.0f), FMath::RandRange(-180.0f, 180.0f), FMath::RandRange(-180.0f, 180.0f));
            bIsTransitioning = true;
            TransitionAlpha = 0.0f;
        }

        if (bIsTransitioning)
        {
            TransitionAlpha += DeltaTime / TransitionDuration;
            if (TransitionAlpha >= 1.0f)
            {
                TransitionAlpha = 1.0f;
                bIsTransitioning = false;
            }
        }

        FVector TargetCenterLoc = CenterActor->GetActorLocation();
        CurrentCenterLocation = FMath::VInterpTo(CurrentCenterLocation, TargetCenterLoc, DeltaTime, OrbitInterpSpeed);
    }
}
