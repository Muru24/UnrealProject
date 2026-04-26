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
	SanitizeSkillSpec(BuffSkill, ESkillType::Buff);
	BuffSkillCooldownRemaining = 0.0f;
}

void USkillComponent::SetOffensiveSkill(const FSkillSpec& InSkillSpec)
{
	OffensiveSkill = InSkillSpec;
	SanitizeSkillSpec(OffensiveSkill, ESkillType::Offensive);
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

void USkillComponent::SanitizeSkillSpec(FSkillSpec& SkillSpec, ESkillType ExpectedSkillType) const
{
	SkillSpec.SkillType = ExpectedSkillType;
	SkillSpec.Cooldown = FMath::Max(0.0f, SkillSpec.Cooldown);
	SkillSpec.Duration = FMath::Max(0.0f, SkillSpec.Duration);
}

bool USkillComponent::TryActivateSkill(const FSkillSpec& SkillSpec, float& CooldownRemaining, ESkillType ExpectedSkillType, AActor* TargetActor)
{
	if (CooldownRemaining > 0.0f || !SkillSpec.IsValid() || !GetOwner())
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

	CooldownRemaining = FMath::Max(0.0f, SkillSpec.Cooldown);
	return true;
}
