#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillTypes.h"
#include "SkillComponent.generated.h"

class AActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetBuffSkill(const FSkillSpec& InSkillSpec);
	void SetOffensiveSkill(const FSkillSpec& InSkillSpec);

	bool TryActivateBuffSkill(AActor* TargetActor = nullptr);
	bool TryActivateOffensiveSkill(AActor* TargetActor = nullptr);

	const FSkillSpec& GetBuffSkill() const { return BuffSkill; }
	const FSkillSpec& GetOffensiveSkill() const { return OffensiveSkill; }

	UFUNCTION(BlueprintPure, Category = "skill")
	float GetBuffSkillCooldownRemaining() const { return BuffSkillCooldownRemaining; }
	UFUNCTION(BlueprintPure, Category = "skill")
	float GetOffensiveSkillCooldownRemaining() const { return OffensiveSkillCooldownRemaining; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skills")
	FSkillSpec BuffSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skills")
	FSkillSpec OffensiveSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skills")
	float BuffSkillCooldownRemaining = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skills")
	float OffensiveSkillCooldownRemaining = 0.0f;

private:
	void SanitizeSkillSpec(FSkillSpec& SkillSpec, ESkillType ExpectedSkillType) const;
	bool TryActivateSkill(const FSkillSpec& SkillSpec, float& CooldownRemaining, ESkillType ExpectedSkillType, AActor* TargetActor);
};
