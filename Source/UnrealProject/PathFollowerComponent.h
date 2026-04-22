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
    virtual void BeginPlay() override;
protected:
    //지정할 경로
    UPROPERTY(EditAnywhere, Category = "Movement")
    AActor* TargetPathActor;

    //이동속도
    UPROPERTY(VisibleAnywhere, Category = "Movement")
    float MoveSpeed = 0;

    //현재 경로 위치
    UPROPERTY(EditAnywhere, Category = "Movement")
    float CurrentDistance = 0.0f;
};
