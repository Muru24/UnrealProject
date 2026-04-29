#pragma once

#include "CoreMinimal.h"
#include "SkillEffectBase.h"
#include "SkillEffect_MultiHomingMissile.generated.h"

class ABulletBase;

UCLASS()
class UNREALPROJECT_API USkillEffect_MultiHomingMissile : public UOffensiveSkillEffect
{
    GENERATED_BODY()

public:
    virtual bool ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Skill")
    TSubclassOf<ABulletBase> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Skill")
    int32 MaxMissiles = 10;

    UPROPERTY(EditAnywhere, Category = "Skill")
    float FireInterval = 0.1f;
};
