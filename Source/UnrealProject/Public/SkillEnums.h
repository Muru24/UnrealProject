#pragma once

#include "CoreMinimal.h"
#include "SkillEnums.generated.h"

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	Buff,
	Offensive
};
