#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossPhaseComponent.h"
#include "SnakeSkillManager.generated.h"

// 스킬 상태를 마스터와 공유하기 위한 열거형 (마스터 헤더에 있던 것 이동 가능)
UENUM(BlueprintType)
enum class ESnakeSkillState : uint8
{
    Idle,
    Forming,
    Attacking,
    Laser_Preparing,
    Laser_Firing
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API USnakeSkillManager : public UActorComponent
{
    GENERATED_BODY()

public:
    USnakeSkillManager();

    // 매 프레임 스킬 상태 관리 및 자동 트리거 체크
    void Update(float DeltaTime, const TArray<class UChildActorComponent*>& Segments);

    // 스킬 강제 발동
    void StartBodyChargeSkill(const TArray<class UChildActorComponent*>& Segments);
    void StartLaserSkill(const TArray<class UChildActorComponent*>& Segments);

    // Getters
    bool IsSkillActive() const { return bIsSkillActive; }
    void ApplyBossPhase(EBossEncounterPhase NewPhase);

    // 설정값들 (관리자로부터 전달받음)
    void SetConfig(float InAutoDelay, float InLaunchDelay, float InFormRadius)
    {
        SkillAutoTriggerDelay = InAutoDelay;
        LaunchDelayBetweenParts = InLaunchDelay;
        FormationRadius = InFormRadius;
    }

protected:
    UPROPERTY(EditAnywhere, Category = "Snake|Skill")
    float SkillAutoTriggerDelay = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Snake|Skill")
    float LaunchDelayBetweenParts = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Snake|Skill")
    float FormationRadius = 400.0f;

    UPROPERTY(EditAnywhere, Category = "Snake|Skill")
    float LaserDuration = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Snake|Skill")
    float LaserPrepareTime = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Snake|Skill")
    float LaserLineSpacing = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Snake|Skill")
    float LaserHeightOffset = 500.0f;

    // 현재 상태 변수들
    float SkillAutoTriggerTimer;
    float StateTimer = 0.0f;
    float SequentialTimer = 0.0f;
    int32 CurrentFiringIndex = 0;
    ESnakeSkillState CurrentSkillPhase = ESnakeSkillState::Idle;
    bool bIsSkillActive = false;
};
