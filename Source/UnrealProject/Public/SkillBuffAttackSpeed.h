#pragma once

#include "CoreMinimal.h"
#include "SkillEffectBase.h"
#include "SkillBuffAttackSpeed.generated.h"

class UNiagaraSystem;

UCLASS()
class UNREALPROJECT_API USkillBuffAttackSpeed : public UBuffSkillEffect
{
	GENERATED_BODY()

public:
	virtual bool ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Attack Speed", meta = (ClampMin = "1.0"))
	float FireRateMultiplier = 1.75f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Attack Speed", meta = (ClampMin = "0.0"))
	float EffectDurationOverride = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Attack Speed")
	TObjectPtr<UNiagaraSystem> ActivationEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Attack Speed")
	bool bAttachEffectToCraft = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Attack Speed")
	FVector EffectLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Attack Speed")
	FVector EffectScale = FVector(1.0f, 1.0f, 1.0f);

private:
	class ASquadCraftActor* ResolveActiveCraft(AActor* SourceActor) const;
	void PlayActivationEffect(AActor* TargetActor, float SkillDuration) const;
};
