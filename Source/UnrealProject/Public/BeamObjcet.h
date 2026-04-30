// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeamObjcet.generated.h"

class ULaserAttackComponent;
UCLASS()
class UNREALPROJECT_API ABeamObjcet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeamObjcet();

	UPROPERTY(EditAnywhere, Category = "Comp")
	UStaticMeshComponent* LaserStart;

	UPROPERTY(EditAnywhere, Category = "Comp")
	UStaticMeshComponent* LaserEnd;

	UPROPERTY(EditAnywhere, Category = "Beam")
	float BeamDistance = 500.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Comp")
	ULaserAttackComponent* LaserEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam|Components")
	TObjectPtr<USceneComponent> FireOrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam|Components")
	TObjectPtr<USceneComponent> FireEnd;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

};
