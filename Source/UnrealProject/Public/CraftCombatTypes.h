#pragma once

#include "CoreMinimal.h"
#include "BulletBase.h"
#include "CraftCombatTypes.generated.h"

class ABulletBase;

UENUM(BlueprintType)
enum class ECraftAttackPattern : uint8
{
	Single,
	Burst,
	Spread
};

USTRUCT(BlueprintType)
struct FCraftAttackConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<ABulletBase> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	EBulletAttackType AttackType = EBulletAttackType::NonPiercing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	ECraftAttackPattern AttackPattern = ECraftAttackPattern::Single;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Piercing", meta = (ClampMin = "0"))
	int32 MaxPenetrationCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Explosive", meta = (ClampMin = "0.0"))
	float ExplosionRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Burst", meta = (ClampMin = "1"))
	int32 BurstCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Spread", meta = (ClampMin = "1"))
	int32 SpreadCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Spread", meta = (ClampMin = "0.0"))
	float SpreadAngle = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Multi Shot", meta = (ClampMin = "0.0"))
	float MultiShotSpacing = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Auto Fire", meta = (ClampMin = "0.05"))
	float AutoFireInterval = 0.2f;
};
