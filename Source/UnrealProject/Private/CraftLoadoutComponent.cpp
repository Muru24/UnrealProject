#include "CraftLoadoutComponent.h"

#include "CraftAttackComponent.h"
#include "SkillComponent.h"

UCraftLoadoutComponent::UCraftLoadoutComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCraftLoadoutComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCraftLoadoutComponent::ApplyLoadoutToAttackComponent(UCraftAttackComponent* AttackComponent) const
{
	if (!AttackComponent)
	{
		return;
	}

	AttackComponent->ApplyLoadoutData(LoadoutData);
}

void UCraftLoadoutComponent::ApplyLoadoutToSkillComponent(USkillComponent* SkillComponent) const
{
	if (!SkillComponent)
	{
		return;
	}

	SkillComponent->SetBuffSkill(LoadoutData.BuffSkill);
	SkillComponent->SetOffensiveSkill(LoadoutData.OffensiveSkill);
}
