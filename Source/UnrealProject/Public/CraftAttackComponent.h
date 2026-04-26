#pragma once

#include "CoreMinimal.h"
#include "BulletBase.h"
#include "Components/ActorComponent.h"
#include "CraftCombatTypes.h"
#include "CraftAttackComponent.generated.h"

class APawn;
class USceneComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UCraftAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCraftAttackComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ApplyAttackConfig(const FCraftAttackConfig& InAttackConfig);
	bool FireFromOrigin(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
	bool TryAutoFireFromOrigin(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);

protected:
	FCraftAttackConfig AttackConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float AutoFireCooldownRemaining = 0.0f;

	FRotator BuildAimRotation(USceneComponent* FireOriginComponent, const FVector& TargetPoint) const;
	bool SpawnProjectile(USceneComponent* FireOriginComponent, const FRotator& SpawnRotation, APawn* InstigatorPawn, float LateralOffset = 0.0f);
	bool FireSingle(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
	bool FireBurst(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
	bool FireSpread(USceneComponent* FireOriginComponent, const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
};
