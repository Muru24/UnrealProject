// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

class UProjectileMovementComponent;
UCLASS()
class UNREALPROJECT_API ABulletBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletBase();

protected:
	virtual void BeginPlay() override;

	//총알 삭제시간
	UPROPERTY(EditAnywhere, Category = "Movement")
	float LifeTime;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float Damage = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Bullet")
	AActor* TargetActor;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	class UBoxComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	class UNiagaraComponent* BulletVisualEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	class UNiagaraSystem* SelectedEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	class UNiagaraSystem* HitEffect;

public:	
	virtual void Tick(float DeltaTime) override;
	void HandleLifeTime(float DeltaTime);
	void ProcessImpact(AActor* OtherActor, const FVector& ImpactLocation);

	void SetTarget(AActor* target) { TargetActor = target; }
	AActor* GetTarget() { return TargetActor; }

	float GetSpeed() { return Speed; }
	float GetMaxSpeed() { return MaxSpeed; }

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
