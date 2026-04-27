#include "CraftLoadoutComponent.h"

#include "CraftAttackComponent.h"
#include "SkillComponent.h"

UCraftLoadoutComponent::UCraftLoadoutComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCraftLoadoutComponent::ApplyLoadoutToAttackComponent(UCraftAttackComponent* AttackComponent) const
{
	if (!AttackComponent)
	{
		return;
	}

	AttackComponent->ApplyAttackConfig(LoadoutData.AttackConfig);
}

void UCraftLoadoutComponent::ApplyLoadoutToSkillComponent(USkillComponent* SkillComponent) const
{
	if (!SkillComponent)
	{
		return;
	}

	if (LoadoutData.SkillConfig.BuffSkill.IsValid())
	{
		SkillComponent->SetBuffSkill(LoadoutData.SkillConfig.BuffSkill);
	}

	if (LoadoutData.SkillConfig.OffensiveSkill.IsValid())
	{
		SkillComponent->SetOffensiveSkill(LoadoutData.SkillConfig.OffensiveSkill);
	}
}
