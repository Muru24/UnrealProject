#pragma once

#include "CoreMinimal.h"
#include "SkillEffectBase.h"
#include "SkillBuffHeal.generated.h"

class UNiagaraSystem;

UCLASS()
class UNREALPROJECT_API USkillBuffHeal : public UBuffSkillEffect
{
	GENERATED_BODY()

public:
	virtual bool ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Heal", meta = (ClampMin = "0.0"))
	float HealAmount = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Heal", meta = (ClampMin = "0.0"))
	float EffectDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Heal")
	TObjectPtr<UNiagaraSystem> ActivationEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Heal")
	bool bAttachEffectToCraft = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Heal")
	FVector EffectLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Heal")
	FVector EffectScale = FVector(1.0f, 1.0f, 1.0f);

private:
	class ASquadCraftActor* ResolveActiveCraft(AActor* SourceActor) const;
	void PlayActivationEffect(AActor* TargetActor) const;
};
