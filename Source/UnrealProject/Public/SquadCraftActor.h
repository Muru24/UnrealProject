#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CraftLoadoutComponent.h"
#include "SquadComponent.h"
#include "SquadCraftActor.generated.h"

UCLASS()
class UNREALPROJECT_API ASquadCraftActor : public AActor
{
	GENERATED_BODY()

public:
	ASquadCraftActor();

	virtual void Tick(float DeltaTime) override;

	void SetAssignedSlot(ESquadSlot InAssignedSlot) { AssignedSlot = InAssignedSlot; }
	ESquadSlot GetAssignedSlot() const { return AssignedSlot; }

	void SetActiveCraft(bool bInIsActiveCraft);
	void SetDesiredRelativeTransform(const FVector& InLocation, const FRotator& InRotation);
	FRotator GetCurrentRelativeRotation() const;
	FVector GetCurrentRelativeLocation() const;
	bool FireAt(const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
	bool TryAutoFireAt(const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);

	USceneComponent* GetFireOrigin() const { return FireOrigin; }
	UStaticMeshComponent* GetCraftMesh() const { return CraftMesh; }
	ECraftCombatRole GetCombatRole() const;
	bool IsActiveCraft() const { return bIsActiveCraft; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* VisualRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CraftMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* FireOrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCraftAttackComponent* AttackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCraftLoadoutComponent* LoadoutComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkillComponent* SkillComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	float TransformInterpSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	FVector ActiveScale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	FVector InactiveScale = FVector(0.92f, 0.92f, 0.92f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Squad")
	bool bIsActiveCraft = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Squad")
	ESquadSlot AssignedSlot = ESquadSlot::Center;

	FVector DesiredRelativeLocation = FVector::ZeroVector;
	FRotator DesiredRelativeRotation = FRotator::ZeroRotator;

private:
	void ApplyLoadout();
};
