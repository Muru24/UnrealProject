#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftCombatTypes.h"
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

USTRUCT(BlueprintType)
struct FCraftIdentityConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FName LoadoutId = TEXT("Default");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	ECraftCombatRole CombatRole = ECraftCombatRole::SupportRapid;
};

USTRUCT(BlueprintType)
struct FCraftSkillConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	FSkillSpec BuffSkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	FSkillSpec OffensiveSkill;
};

USTRUCT(BlueprintType)
struct FCraftPresentationConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
	FName VisualProfileId = TEXT("Default");
};

USTRUCT(BlueprintType)
struct FCraftLoadoutData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	FCraftIdentityConfig Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	FCraftAttackConfig AttackConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	FCraftSkillConfig SkillConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	FCraftPresentationConfig PresentationConfig;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UCraftLoadoutComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCraftLoadoutComponent();

	void ApplyLoadoutToAttackComponent(UCraftAttackComponent* AttackComponent) const;
	void ApplyLoadoutToSkillComponent(USkillComponent* SkillComponent) const;
	const FCraftLoadoutData& GetLoadoutData() const { return LoadoutData; }
	const FCraftIdentityConfig& GetIdentityConfig() const { return LoadoutData.Identity; }
	const FCraftAttackConfig& GetAttackConfig() const { return LoadoutData.AttackConfig; }
	const FCraftSkillConfig& GetSkillConfig() const { return LoadoutData.SkillConfig; }
	const FCraftPresentationConfig& GetPresentationConfig() const { return LoadoutData.PresentationConfig; }
	ECraftCombatRole GetCombatRole() const { return LoadoutData.Identity.CombatRole; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	FCraftLoadoutData LoadoutData;
};
