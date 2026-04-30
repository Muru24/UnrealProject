// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shild.generated.h"

class UStatComponent;
UCLASS()
class UNREALPROJECT_API AShild : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShild();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Comp")
	float LifeTime;

	UPROPERTY(EditAnywhere, Category = "Comp")
	UStatComponent* State;

	UPROPERTY(BlueprintReadWrite, Category = "Comp")
	float CurentTime = 0;

	virtual void Tick(float DeltaTime) override;

};
