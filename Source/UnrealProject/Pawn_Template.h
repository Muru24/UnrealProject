// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnStruct.h"
#include "Pawn_Template.generated.h"

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

	//±âº» À¯´Ö ÇÊ¼ö ÄÄÆ÷³ÍÆ®
	// Ãæµ¹ ÄÄÆ÷³ÍÆ®
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	 USphereComponent* CollisionComponent;

	//¿ÜÇü ÇÁ¸®ÆÕ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	 UStaticMeshComponent* PlaneMesh; 

	//¿òÁ÷ÀÌ´Â À¯´Ö ½ºÅÝ
	 UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	 class UStatComponent* StatComponent;


public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Fire() {};
};
