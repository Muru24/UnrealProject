#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SnakePathController.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API USnakePathController : public UActorComponent
{
    GENERATED_BODY()

public:
    USnakePathController();

    void UpdatePath(float DeltaTime, AActor* CenterActor, bool bPaused);

    FVector GetCurrentCenterLocation() const { return CurrentCenterLocation; }
    FRotator GetSourceOrbitRotation() const { return SourceOrbitRotation; }
    FRotator GetTargetOrbitRotation() const { return TargetOrbitRotation; }
    float GetTransitionAlpha() const { return TransitionAlpha; }

    void SetOrbitInterpSpeed(float Speed) { OrbitInterpSpeed = Speed; }
    void SetPathChangeInterval(float Interval) { PathChangeInterval = Interval; }

protected:
    UPROPERTY(EditAnywhere, Category = "Snake|Path")
    float PathChangeInterval = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Snake|Path")
    float TransitionDuration = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Snake|Path")
    float OrbitInterpSpeed = 2.0f;

    // 현재 상태 변수들
    FVector CurrentCenterLocation;
    FRotator SourceOrbitRotation;
    FRotator TargetOrbitRotation;
    float TransitionAlpha = 1.0f;
    float PathChangeTimer = 0.0f;
    bool bIsTransitioning = false;
};
