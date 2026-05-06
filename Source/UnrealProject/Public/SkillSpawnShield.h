#pragma once

#include "CoreMinimal.h"
#include "SkillEffectBase.h"
#include "SkillSpawnShield.generated.h"

class AShild;

/**
 * 편대 중앙 기체의 전방에 방패(Shield)를 소환하고 부착하는 스킬입니다.
 */
UCLASS()
class UNREALPROJECT_API USkillSpawnShield : public UBuffSkillEffect
{
	GENERATED_BODY()

public:
	virtual bool ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration) override;

protected:
	/** 소환할 방패 클래스 (AShild 기반) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Shield")
	TSubclassOf<AShild> ShieldClass;

	/** 센터 기체로부터 전방으로 소환될 거리 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Shield")
	float ForwardOffset = 150.0f;

private:
	/** 편대의 현재 활성화된 기체(Active)를 찾아 반환합니다. */
	AActor* ResolveActiveActor(AActor* SourceActor) const;
};
