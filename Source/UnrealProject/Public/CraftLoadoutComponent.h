#pragma once

#include "CoreMinimal.h"
#include "BulletBase.h"
#include "Components/ActorComponent.h"
#include "SkillTypes.h"
#include "CraftLoadoutComponent.generated.h"

class UCraftAttackComponent;
class USkillComponent;

UENUM(BlueprintType)
enum class ECraftCombatRole : uint8
{
	MainGun,
	SupportRapid,
	SupportHeavy
};

UENUM(BlueprintType)
enum class ECraftAttackPattern : uint8
{
	Single,
	Burst,
	Spread
};

USTRUCT(BlueprintType)
struct FCraftLoadoutData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	FName LoadoutId = TEXT("Default");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	ECraftCombatRole CombatRole = ECraftCombatRole::SupportRapid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	TSubclassOf<ABulletBase> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	EBulletAttackType AttackType = EBulletAttackType::NonPiercing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	ECraftAttackPattern AttackPattern = ECraftAttackPattern::Single;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout|Piercing", meta = (ClampMin = "0"))
	int32 MaxPenetrationCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout|Explosive", meta = (ClampMin = "0.0"))
	float ExplosionRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout|Burst", meta = (ClampMin = "1"))
	int32 BurstCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout|Spread", meta = (ClampMin = "1"))
	int32 SpreadCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout|Spread", meta = (ClampMin = "0.0"))
	float SpreadAngle = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout|Multi Shot", meta = (ClampMin = "0.0"))
	float MultiShotSpacing = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout|Auto Fire", meta = (ClampMin = "0.05"))
	float AutoFireInterval = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout|Skills")
	FSkillSpec BuffSkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout|Skills")
	FSkillSpec OffensiveSkill;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UCraftLoadoutComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCraftLoadoutComponent();

	virtual void BeginPlay() override;

	void ApplyLoadoutToAttackComponent(UCraftAttackComponent* AttackComponent) const;
	void ApplyLoadoutToSkillComponent(USkillComponent* SkillComponent) const;
	const FCraftLoadoutData& GetLoadoutData() const { return LoadoutData; }
	ECraftCombatRole GetCombatRole() const { return LoadoutData.CombatRole; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	FCraftLoadoutData LoadoutData;
};
