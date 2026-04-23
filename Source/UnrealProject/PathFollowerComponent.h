// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PathFollowerComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UPathFollowerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPathFollowerComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void SetTargetPath(AActor* InPathActor) { TargetPathActor = InPathActor; }
    void SetPathSpeed(float Speed) { MoveSpeed = Speed; }
    void SetApplyOwnerTransform(bool bInApplyOwnerTransform) { bApplyOwnerTransform = bInApplyOwnerTransform; }
    bool HasValidBaseTransform() const { return bHasValidBaseTransform; }
    const FTransform& GetBaseWorldTransform() const { return BaseWorldTransform; }
    virtual void BeginPlay() override;

protected:
    //지정할 경로
    UPROPERTY(EditAnywhere, Category = "Movement")
    AActor* TargetPathActor;

    //이동속도
    UPROPERTY(VisibleAnywhere, Category = "Movement")
    float MoveSpeed = 0;

    //속도 보간 수치
    UPROPERTY(VisibleAnywhere, Category = "Movement")
    float LocationInterpSpeed = 15.0f;

    //좌우 뱅킹 강도
    UPROPERTY(EditAnywhere, Category = "Movement")
    float BankingIntensity = 1.5f;

    //회전 속도
    UPROPERTY(EditAnywhere, Category = "Movement")
    float RotationInterpSpeed = 3.0f;

    //뱅킹 부드러움
    UPROPERTY(EditAnywhere, Category = "Movement|Banking")
    float BankingInterpSpeed = 2.0f; 

    //상하 뱅킹
    UPROPERTY(EditAnywhere, Category = "Movement|Banking")
    float PitchExaggeration = 1.2f; 

    //시작 위치
    UPROPERTY(EditAnywhere, Category = "Movement|Banking")
    float CurrentDistance = 0.0f;

    // 타겟 Yaw 변화량 (뱅킹 계산용)
    float InterpolatedYawDelta = 0.0f; 

    // 현재 Roll 값 (부드러운 뱅킹용)
    float CurrentRoll = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    bool bApplyOwnerTransform = true;

    UPROPERTY(VisibleAnywhere, Category = "Movement")
    FTransform BaseWorldTransform;

    bool bHasValidBaseTransform = false;
};
