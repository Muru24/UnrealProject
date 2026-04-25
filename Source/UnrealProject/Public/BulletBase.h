// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

class UProjectileMovementComponent;

UENUM(BlueprintType)
enum class EBulletAttackType : uint8
{
	NonPiercing,
	Piercing,
	Explosive
};

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	EBulletAttackType AttackType = EBulletAttackType::NonPiercing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Piercing", meta = (ClampMin = "0"))
	int32 RemainingPenetrations = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Explosive", meta = (ClampMin = "0.0"))
	float ExplosionRadius = 0.0f;

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
	void ConfigureAttackType(EBulletAttackType InAttackType, int32 InRemainingPenetrations, float InExplosionRadius);

	float GetSpeed() { return Speed; }
	float GetMaxSpeed() { return MaxSpeed; }

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	bool ApplyDamageToActor(AActor* TargetActorToDamage);
	void ApplyExplosionDamage(const FVector& ImpactLocation, AActor* DirectHitActor);
	bool CanAffectActor(AActor* OtherActor) const;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> DamagedActors;
};
