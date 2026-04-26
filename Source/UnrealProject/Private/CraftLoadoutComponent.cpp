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

	SkillComponent->SetBuffSkill(LoadoutData.SkillConfig.BuffSkill);
	SkillComponent->SetOffensiveSkill(LoadoutData.SkillConfig.OffensiveSkill);
}
