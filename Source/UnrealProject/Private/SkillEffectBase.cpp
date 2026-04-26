#include "SkillEffectBase.h"

bool USkillEffectBase::ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration)
{
	const bool bHasValidSource = IsValid(SourceActor);
	const bool bHasValidTarget = (TargetActor == nullptr) || IsValid(TargetActor);
	return bHasValidSource && bHasValidTarget;
}

ESkillType USkillEffectBase::GetSkillType_Implementation() const
{
	return ESkillType::Buff;
}

ESkillType UBuffSkillEffect::GetSkillType_Implementation() const
{
	return ESkillType::Buff;
}

ESkillType UOffensiveSkillEffect::GetSkillType_Implementation() const
{
	return ESkillType::Offensive;
}
