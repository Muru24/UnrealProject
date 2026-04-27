// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillEffectBase.h"
#include "SkillSummonPanel.generated.h"

class AFunnel;

UCLASS()
class UNREALPROJECT_API USkillSummonPanel : public UOffensiveSkillEffect
{
	GENERATED_BODY()

public:
	virtual bool ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Funnel")
	TSubclassOf<AFunnel> FunnelClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Funnel", meta = (ClampMin = "1"))
	int32 FunnelCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Funnel", meta = (ClampMin = "0.0"))
	float FunnelSpawnRadius = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Funnel")
	FVector FunnelSpawnOffset = FVector(0.0f, 0.0f, 40.0f);

private:
	AActor* ResolveTargetActor(AActor* SourceActor, AActor* RequestedTarget) const;
	AActor* ResolveLockOnTarget(AActor* SourceActor) const;
	AActor* ResolveNearestEnemy(AActor* SourceActor) const;
};
