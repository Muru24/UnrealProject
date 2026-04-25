#include "SkillComponent.h"

#include "SkillEffectBase.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	BuffSkillCooldownRemaining = FMath::Max(0.0f, BuffSkillCooldownRemaining - DeltaTime);
	OffensiveSkillCooldownRemaining = FMath::Max(0.0f, OffensiveSkillCooldownRemaining - DeltaTime);
}

void USkillComponent::SetBuffSkill(const FSkillSpec& InSkillSpec)
{
	BuffSkill = InSkillSpec;
	BuffSkill.SkillType = ESkillType::Buff;
	BuffSkillCooldownRemaining = 0.0f;
}

void USkillComponent::SetOffensiveSkill(const FSkillSpec& InSkillSpec)
{
	OffensiveSkill = InSkillSpec;
	OffensiveSkill.SkillType = ESkillType::Offensive;
	OffensiveSkillCooldownRemaining = 0.0f;
}

bool USkillComponent::TryActivateBuffSkill(AActor* TargetActor)
{
	return TryActivateSkill(BuffSkill, BuffSkillCooldownRemaining, ESkillType::Buff, TargetActor);
}

bool USkillComponent::TryActivateOffensiveSkill(AActor* TargetActor)
{
	return TryActivateSkill(OffensiveSkill, OffensiveSkillCooldownRemaining, ESkillType::Offensive, TargetActor);
}

bool USkillComponent::TryActivateSkill(const FSkillSpec& SkillSpec, float& CooldownRemaining, ESkillType ExpectedSkillType, AActor* TargetActor)
{
	if (CooldownRemaining > 0.0f || !SkillSpec.IsValid())
	{
		return false;
	}

	USkillEffectBase* SkillEffect = NewObject<USkillEffectBase>(this, SkillSpec.EffectClass);
	if (!SkillEffect || SkillEffect->GetSkillType() != ExpectedSkillType)
	{
		return false;
	}

	if (!SkillEffect->ActivateSkill(GetOwner(), TargetActor, SkillSpec.Duration))
	{
		return false;
	}

	CooldownRemaining = SkillSpec.Cooldown;
	return true;
}
