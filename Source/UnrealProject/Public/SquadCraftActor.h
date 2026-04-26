#pragma once

#include "CoreMinimal.h"
#include "CraftLoadoutComponent.h"
#include "GameFramework/Actor.h"
#include "SquadComponent.h"
#include "SquadCraftActor.generated.h"

UCLASS()
class UNREALPROJECT_API ASquadCraftActor : public AActor
{
	GENERATED_BODY()

public:
	ASquadCraftActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SetAssignedSlot(ESquadSlot InAssignedSlot) { AssignedSlot = InAssignedSlot; }
	ESquadSlot GetAssignedSlot() const { return AssignedSlot; }

	void SetActiveCraft(bool bInIsActiveCraft);
	void SetDesiredRelativeTransform(const FVector& InLocation, const FRotator& InRotation);
	void SetVisualTiltRotation(const FRotator& InRotation);
	FRotator GetCurrentRelativeRotation() const;
	FVector GetCurrentRelativeLocation() const;

	bool FireAt(const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);
	bool TryAutoFireAt(const FVector& TargetPoint, AActor* TargetActor, APawn* InstigatorPawn);

	USceneComponent* GetFireOrigin() const { return FireOrigin; }
	UStaticMeshComponent* GetCraftMesh() const { return CraftMesh; }
	ECraftCombatRole GetCombatRole() const;
	bool IsActiveCraft() const { return bIsActiveCraft; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> VisualRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CraftMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> FireOrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCraftAttackComponent> AttackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCraftLoadoutComponent> LoadoutComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USkillComponent> SkillComponent;

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
	FRotator DesiredMeshTiltRotation = FRotator::ZeroRotator;

private:
	void ApplyLoadout();
};
