#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

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

	UPROPERTY(EditAnywhere, Category = "Movement")
	float LifeTime = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 1500.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SpeedRampDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SpeedRampInterpSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float Damage = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	EBulletAttackType AttackType = EBulletAttackType::NonPiercing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Piercing", meta = (ClampMin = "0"))
	int32 RemainingPenetrations = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Explosive", meta = (ClampMin = "0.0"))
	float ExplosionRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Damageable")
	bool bCanBeShotDown = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Damageable", meta = (ClampMin = "0.0"))
	float MaxHitPoints = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Bullet")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<class UBoxComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<class UNiagaraComponent> BulletVisualEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<class UNiagaraSystem> SelectedEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<class UNiagaraSystem> HitEffect;

public:
	virtual void Tick(float DeltaTime) override;
	void HandleLifeTime(float DeltaTime);
	void ProcessImpact(AActor* OtherActor, const FVector& ImpactLocation);

	void SetTarget(AActor* InTargetActor) { TargetActor = InTargetActor; }
	AActor* GetTarget() const { return TargetActor.Get(); }
	void ConfigureAttackType(EBulletAttackType InAttackType, int32 InRemainingPenetrations, float InExplosionRadius);
	void SetProjectileSpeed(float InSpeed, float InMaxSpeed);
	void ConfigureSpeedRamp(float InSpeedRampDelay, float InSpeedRampInterpSpeed);
	void ConfigureShotDown(float InMaxHitPoints, bool bInCanBeShotDown);
	bool CanBeShotDown() const { return bCanBeShotDown; }

	float GetSpeed() const { return CurrentMoveSpeed; }
	float GetMaxSpeed() const { return MaxSpeed; }

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	bool ApplyDamageToActor(AActor* TargetActorToDamage);
	bool ApplyBulletDamage(float DamageAmount);
	void ApplyExplosionDamage(const FVector& ImpactLocation, AActor* DirectHitActor);
	bool CanAffectActor(AActor* OtherActor) const;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> DamagedActors;

	float CurrentHitPoints = 0.0f;
	float CurrentMoveSpeed = 0.0f;
	float SpeedRampElapsedTime = 0.0f;
};
