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

    UPROPERTY(EditAnywhere, Category = "Skill|Missile")
    float MissileMoveSpeed = 1400.0f;

    UPROPERTY(EditAnywhere, Category = "Skill|Missile")
    float MissileMaxSpeed = 1800.0f;

    UPROPERTY(EditAnywhere, Category = "Skill|Missile")
    float MissileTurnInterpSpeed = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Skill|Missile")
    float MissileInitialFireDelay = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Skill|Missile")
    float MissileLaunchSpreadAngleDegrees = 4.0f;

    UPROPERTY(EditAnywhere, Category = "Skill|Missile")
    float MissileLaunchPitchSpreadAngleDegrees = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Skill|Missile")
    float MissileHomingActivationDelay = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Skill|Missile")
    float MissileSpeedRampDelay = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Skill|Missile")
    float MissileSpeedRampInterpSpeed = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Skill|Missile")
    float MissileHitPoints = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Skill|Missile")
    bool bMissilesCanBeShotDown = false;
};
