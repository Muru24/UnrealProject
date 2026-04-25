#include "SkillEffectBase.h"

bool USkillEffectBase::ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration)
{
	return SourceActor != nullptr;
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
