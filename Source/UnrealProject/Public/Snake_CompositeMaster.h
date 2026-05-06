// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn_CompositeMaster.h"
#include "Snake_CompositeMaster.generated.h"

/**
 * 
 */
class USplineComponent;
class USnakePathController;
class USnakeSkillManager;
enum class EBossEncounterPhase : uint8;
// ESnakeSkillPhase는 SnakeSkillManager.h로 이동되었습니다.

UCLASS()
class UNREALPROJECT_API ASnake_CompositeMaster : public APawn_CompositeMaster
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USnakePathController* PathController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USnakeBodyChargeComponent* HeadSkillComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USnakeSkillManager* SkillManager;

public:
	ASnake_CompositeMaster();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void ApplyBossPhase(EBossEncounterPhase NewPhase);

    // 궤도 위치 및 축 계산을 위한 헬퍼 함수
    void GetOrbitTransform(float Angle, const FRotator& Rotation, FVector& OutLocation, FVector& OutForward, FVector& OutRight, FVector& OutUp, const FVector& CenterLoc);

	UPROPERTY(EditAnywhere, Category = "Snake|Orbit")
	AActor* CenterActor;

	UPROPERTY(EditAnywhere, Category = "Snake|Orbit")
	float OrbitRadius;

	UPROPERTY(EditAnywhere, Category = "Snake|Orbit")
	float OrbitSpeed;

	UPROPERTY(EditAnywhere, Category = "Snake|Orbit")
	float SegmentAngleGap;

    UPROPERTY(EditAnywhere, Category = "Snake|Spiral")
    bool RunPatten = false;

    UPROPERTY(EditAnywhere, Category = "Snake|Spiral", meta = (ToolTip = "마디가 사라졌을 때 줄어드는 속도"))
    float ShrinkSpeed = 5.0f;

    TArray<float> SegAngleOffsets;

    float CurrentOrbitAngle;

    // 이벤트 기반 마디 관리: 마디 파괴 시 호출됨
    void UnregisterSegment(class UChildActorComponent* SegmentComp);
};
