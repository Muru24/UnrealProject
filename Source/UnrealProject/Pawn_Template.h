// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnStruct.h"
#include "Pawn_Template.generated.h"

class ABulletBase;
class USphereComponent;
class UStaticMeshComponent;
class UPathFollowerComponent;

UCLASS()
class UNREALPROJECT_API APawn_Template : public APawn
{
	GENERATED_BODY()

public:
	APawn_Template();

protected:
	virtual void BeginPlay() override;


	//비행기 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	 USphereComponent* CollisionComponent;

	//비행기 외형 프리팹
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	 UStaticMeshComponent* PlaneMesh; 

	//경로 따라가는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	 UPathFollowerComponent* PathFollower;

	//움직이는 유닛 스텟
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ShowOnlyInnerProperties), Category = "Pawn Setting")
	FMovePawn PawnState;

	//총알 발사 위치 배열
	UPROPERTY()
	TArray<USceneComponent*> FirePoints;

	 //총알 클래스
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<ABulletBase> BulletBase;

public:	
	virtual void Tick(float DeltaTime) override;


	void Initialize_GameManager_Pawn();

	void Fire();

};
