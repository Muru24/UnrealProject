#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LaserAttackComponent.generated.h"

class ABeamEffectActor;
class USceneComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API ULaserAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULaserAttackComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Laser")
	void StartLaser(AActor* InTarget);

	UFUNCTION(BlueprintCallable, Category = "Laser")
	void StartLaserFromComponent(USceneComponent* InTargetComponent);

	UFUNCTION(BlueprintCallable, Category = "Laser")
	void StopLaser();

	UFUNCTION(BlueprintCallable, Category = "Laser")
	void SetTargetActor(AActor* InTargetActor);

	UFUNCTION(BlueprintCallable, Category = "Laser")
	void SetTargetComponent(USceneComponent* InTargetComponent);

	void SetFireOrigin(USceneComponent* InOrigin) { FireOrigin = InOrigin; }
	void SetBeamExtraDistance(float InDistance) { BeamExtraDistance = InDistance; }
	void SetAttackActiveDuration(float InDuration) { AttackActiveDuration = InDuration; }
	void SetWarningDuration(float InDuration) { WarningDuration = InDuration; }
	AActor* GetTargetActor() const { return TargetActor; }
	float GetAttackActiveDuration() const { return AttackActiveDuration; }
	float GetWarningDuration() const { return WarningDuration; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
	TSubclassOf<ABeamEffectActor> BeamActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
	float AttackActiveDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
	float WarningDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config", meta = (ClampMin = "0.0"))
	float WarningFinalPulseDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
	float InitialBeamSize = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
	float FinalBeamSize = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
	float WarningBeamSize = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config", meta = (ClampMin = "1.0"))
	float WarningFinalPulseSizeMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
	FLinearColor WarningBeamColor = FLinearColor(1.0f, 0.05f, 0.05f, 0.2f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
	FLinearColor WarningFinalPulseColor = FLinearColor(1.0f, 0.12f, 0.12f, 0.55f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
	FLinearColor AttackBeamColor = FLinearColor(0.0f, 0.28f, 0.7f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config", meta = (ClampMin = "0.0"))
	float AttackTrackingInterpSpeed = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
	float BeamExtraDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Damage", meta = (ClampMin = "0.0"))
	float AttackDamage = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Damage", meta = (ClampMin = "0.0"))
	float AttackDamageTickInterval = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Damage", meta = (ClampMin = "0.0"))
	float AttackTraceRadius = 45.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Laser|Runtime")
	TObjectPtr<ABeamEffectActor> ActiveBeamActor;

	UPROPERTY(BlueprintReadOnly, Category = "Laser|Runtime")
	bool bAttackActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Laser|Runtime")
	bool bWarningActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Laser|Runtime")
	float AttackTimeTracker = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Laser|Runtime")
	float WarningTimeTracker = 0.0f;

	UPROPERTY()
	TObjectPtr<AActor> TargetActor;

	UPROPERTY()
	TObjectPtr<USceneComponent> TargetComponent;

	UPROPERTY()
	TObjectPtr<USceneComponent> FireOrigin;

	FVector CurrentBeamAimLocation = FVector::ZeroVector;
	bool bHasLockedAimLocation = false;

	FTimerHandle AttackStopTimerHandle;
	FTimerHandle WarningFinishTimerHandle;

	void FireBeam();
	void BeginLaserWarning();
	void BeginLaserFiring();
	void DeactivateActiveBeam();
	FVector ResolveCurrentAimLocation() const;
	FVector GetTargetLocationWithExtension() const;
	FVector GetFireOriginLocation() const;
};
