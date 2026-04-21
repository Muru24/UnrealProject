// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

UCLASS()
class UNREALPROJECT_API ABulletBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//날아갈 방향
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	FVector MoveDirection;

	//총알 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BulletSpeed = 2000.0f;

	//총알 삭제시간
	UPROPERTY(EditAnywhere, Category = "Movement")
	float LifeTime = 5.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void InitBullet(FVector Direction);
	void UpdateStraightMovement(float DeltaTime);
	void HandleLifeTime(float DeltaTime);
};
