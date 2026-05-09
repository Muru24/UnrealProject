#include "StatComponent.h"

UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStatComponent::ApplyDamage(float DamageAmount)
{
	Stats.unit.HP = FMath::Clamp(Stats.unit.HP - DamageAmount, 0.0f, Stats.unit.MaxHP);
	OnHpChanged.Broadcast(Stats.unit.HP);

	if (Stats.unit.HP <= 0.0f)
	{
		//GetOwner()->Destroy();
	}
}

void UStatComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f)
	{
		return;
	}

	Stats.unit.HP = FMath::Clamp(Stats.unit.HP + HealAmount, 0.0f, Stats.unit.MaxHP);
	OnHpChanged.Broadcast(Stats.unit.HP);
}
