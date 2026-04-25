#pragma once

#include "CoreMinimal.h"
#include "SkillEnums.h"
#include "UObject/Object.h"
#include "SkillEffectBase.generated.h"

class AActor;

UCLASS(Blueprintable, Abstract, EditInlineNew, DefaultToInstanced)
class UNREALPROJECT_API USkillEffectBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Skill")
	bool ActivateSkill(AActor* SourceActor, AActor* TargetActor, float SkillDuration);

	UFUNCTION(BlueprintNativeEvent, Category = "Skill")
	ESkillType GetSkillType() const;
};

UCLASS(Blueprintable, Abstract, EditInlineNew, DefaultToInstanced)
class UNREALPROJECT_API UBuffSkillEffect : public USkillEffectBase
{
	GENERATED_BODY()

public:
	virtual ESkillType GetSkillType_Implementation() const override;
};

UCLASS(Blueprintable, Abstract, EditInlineNew, DefaultToInstanced)
class UNREALPROJECT_API UOffensiveSkillEffect : public USkillEffectBase
{
	GENERATED_BODY()

public:
	virtual ESkillType GetSkillType_Implementation() const override;
};
