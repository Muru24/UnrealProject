// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ChildActorComponent.h"
#include "Pawn_CompositeMaster.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPROJECT_API APawn_CompositeMaster : public AActor
{
	GENERATED_BODY()
	
public:
	APawn_CompositeMaster();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
	TArray<UChildActorComponent*> ChildParts;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;

};
