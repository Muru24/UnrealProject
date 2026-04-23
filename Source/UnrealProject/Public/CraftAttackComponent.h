#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftAttackComponent.generated.h"

class ABulletBase;
class APawn;
class USceneComponent;

UENUM(BlueprintType)
enum class ECraftAttackPattern : uint8
{
	Single,
	Burst,
	Spread
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UCraftAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCraftAttackComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool FireFromOrigin(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
	bool TryAutoFireFromOrigin(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<ABulletBase> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	ECraftAttackPattern AttackPattern = ECraftAttackPattern::Single;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Burst", meta = (ClampMin = "1"))
	int32 BurstCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Spread", meta = (ClampMin = "1"))
	int32 SpreadCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Spread", meta = (ClampMin = "0.0"))
	float SpreadAngle = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Multi Shot", meta = (ClampMin = "0.0"))
	float MultiShotSpacing = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.05"))
	float AutoFireInterval = 0.35f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float AutoFireCooldownRemaining = 0.0f;

	bool SpawnProjectile(USceneComponent* FireOriginComponent, const FRotator& SpawnRotation, AActor* TargetActor, APawn* InstigatorPawn, float LateralOffset = 0.0f);
	bool FireSingle(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
	bool FireBurst(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
	bool FireSpread(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
};
