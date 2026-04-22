// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BulletWeaponComponent.generated.h"


class ABulletBase;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPROJECT_API UBulletWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UBulletWeaponComponent();

protected:
	virtual void BeginPlay() override;

	//총알 발사 위치 배열
	UPROPERTY()
	TArray<USceneComponent*> FirePoints;

	//총알 클래스
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<ABulletBase> Prefab;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Fire(FVector TargetPoint, AActor* TargetActor);
};
