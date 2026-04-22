// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


class UEnemyManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPROJECT_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULockOnComponent();

	//록온 타겟 찾는 함수
	void TraceTarget();

	//타겟 변경 함수
	void ChangeTarget();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;

public:
	//록 온 여부
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category ="LockOn")
	bool isLockOn = false;
	
	//현재 타겟
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	APawn* target;

	//현재 타겟
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	int32 targetIndex;

	//몬스터들의 정보를 담고있는 매니저 참조
	UEnemyManager* EnemyManager;
};
