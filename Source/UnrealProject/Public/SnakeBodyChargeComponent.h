#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SnakeBodyChargeComponent.generated.h"

UENUM(BlueprintType)
enum class ESnakePartSkillState : uint8
{
    Idle,
    Forming,
    Flying,
    Returning,
    LaserFiring
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API USnakeBodyChargeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USnakeBodyChargeComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void ActivateSkill(AActor* InTarget);
    
    // 스킬 초기 설정 (목표 위치, 회전, 발사 지연 시간 등)
    void InitSkillSequence(AActor* InTarget, const FVector& InFormLoc, const FRotator& InFormRot, float InLaunchDelay);

    // 레이저 스킬 발동 명령
    void PrepareLaser(AActor* InTarget, const FVector& InFormLoc, const FRotator& InFormRot);
    void FireLaser();
    void StopLaser();

    // 발사 시작 신호
    void SignalLaunch() { bLaunchSignaled = true; }

    // 마스터가 계산한 궤도 정보를 저장 (나선형 이동 계산을 위해 상세 정보 수령)
    void UpdateTargetInfo(const FVector& InBaseLoc, const FVector& InOutward, const FVector& InUp, float InAngle, const FRotator& InRot);

    void SetSkillState(ESnakePartSkillState NewState) { CurrentState = NewState; }
    ESnakePartSkillState GetCurrentState() const { return CurrentState; }
    bool IsReachedFormation() const { return bReachedFormation; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class ULaserAttackComponent* LaserComponent;

    UPROPERTY(BlueprintReadOnly, Category = "Snake|Skill")
    ESnakePartSkillState CurrentState = ESnakePartSkillState::Idle;

    bool bReachedFormation = false;

    UPROPERTY(EditAnywhere, Category = "SnakeSkill")
    float FlySpeed = 2000.0f;

    UPROPERTY(EditAnywhere, Category = "SnakeSkill")
    float ReturnSpeed = 1000.0f;

    UPROPERTY(EditAnywhere, Category = "SnakeSkill")
    float SkillDuration = 3.0f;

    // 나선형 이동(Slithering) 속성
    UPROPERTY(EditAnywhere, Category = "Snake|Spiral")
    float SpiralSpeed = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Snake|Spiral")
    float CrawlSpeed = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Snake|Spiral")
    float SlitherRadius = 50.0f;

    UPROPERTY()
    AActor* TargetActor;

    // 자율 시퀀스용 변수
    FVector FormationLocation;
    FRotator FormationRotation;
    float SequentialLaunchDelay = 0.0f;
    bool bLaunchSignaled = false;
    float StateTimer = 0.0f;

    // 복귀 시 부드러운 합류를 위한 위치 보간용
    // 궤도 정보 저장을 위한 변수들
    FVector TargetBaseLocation;
    FVector OrbitOutward;
    FVector OrbitUp;
    float OrbitAngle = 0.0f;
    FRotator TargetBaseRotation;

    FVector GetSlitherLocation(const FVector& BaseLocation, const FVector& Outward, const FVector& Up, float Angle);

    FVector FlightTargetLocation;
    
    FVector TargetReturnLocation;
    FRotator TargetReturnRotation;
};
