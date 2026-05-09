#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeamEffectActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USceneComponent;
class UPrimitiveComponent;

UCLASS(Blueprintable)
class UNREALPROJECT_API ABeamEffectActor : public AActor
{
	GENERATED_BODY()

public:
	ABeamEffectActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam|Components")
	TObjectPtr<UNiagaraComponent> BeamComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam|Components")
	TObjectPtr<UNiagaraComponent> HitComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Visual")
	FLinearColor MainColor = FLinearColor(0.0f, 0.28f, 0.7f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Visual", meta = (ClampMin = "0.0"))
	float BeamSize = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Runtime")
	FVector BeamEnd = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Parameters")
	FName MainColorParameterName = TEXT("User.mainColor");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Parameters")
	FName BeamEndParameterName = TEXT("User.beamEnd");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Parameters")
	FName BeamSizeParameterName = TEXT("User.size");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Parameters")
	bool bBeamEndUsesWorldSpace = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Assets")
	TObjectPtr<UNiagaraSystem> DefaultBeamSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Assets")
	TObjectPtr<UNiagaraSystem> DefaultHitSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Damage")
	bool bBeamDamageEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Damage", meta = (ClampMin = "0.0"))
	float BeamDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Damage", meta = (ClampMin = "0.0"))
	float BeamDamageTickInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Damage", meta = (ClampMin = "0.0"))
	float BeamTraceRadius = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam|Damage")
	bool bAllowRepeatedDamageToSameTarget = true;

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void ApplyBeamParameters();

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void ApplyBeamColor();

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void ApplyBeamEnd();

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void ApplyBeamSize();

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void SetMainColor(const FLinearColor& InMainColor, bool bApplyImmediately = true);

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void SetBeamEnd(const FVector& InBeamEnd, bool bApplyImmediately = true);

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void SetBeamSize(float InBeamSize, bool bApplyImmediately = true);

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void SetBeamSystem(UNiagaraSystem* InBeamSystem, bool bReinitialize = true);

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void SetHitSystem(UNiagaraSystem* InHitSystem, bool bReinitialize = true);

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void ActivateBeam(bool bReset = false);

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void DeactivateBeam();

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void ActivateHitEffect(bool bReset = true);

	UFUNCTION(BlueprintCallable, Category = "Beam")
	void DeactivateHitEffect();

	UFUNCTION(BlueprintCallable, Category = "Beam|Damage")
	void SetBeamDamageEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Beam|Damage")
	void ConfigureBeamDamage(float InDamage, float InTickInterval, float InTraceRadius, bool bInAllowRepeatedDamageToSameTarget);

	UFUNCTION(BlueprintPure, Category = "Beam")
	UNiagaraComponent* GetBeamComponent() const { return BeamComponent; }

	UFUNCTION(BlueprintPure, Category = "Beam")
	UNiagaraComponent* GetHitComponent() const { return HitComponent; }

protected:
	float BeamDamageTickAccumulator = 0.0f;
	TSet<TWeakObjectPtr<AActor>> DamagedActorsThisActivation;

	void ApplyBeamDamage();
	bool TryApplyDamageToActor(AActor* TargetActor);
	bool CanDamageActor(AActor* OtherActor) const;
};
