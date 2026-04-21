// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnStruct.generated.h"

USTRUCT(BlueprintType)
struct FMoveState
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration;
};


USTRUCT(BlueprintType)
struct FUnitState
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;

};

USTRUCT(BlueprintType)
struct FObjectState
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ID;

};

USTRUCT(BlueprintType)
struct FMovePawn
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PawnState")
	FObjectState object;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PawnState")
	FUnitState unit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PawnState")
	FMoveState move;
};