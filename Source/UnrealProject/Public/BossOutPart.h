#pragma once

#include "CoreMinimal.h"
#include "UnrealProject/Pawn_Template.h"
#include "BossOutPart.generated.h"

class USceneComponent;
class UCraftAttackComponent;
class ULaserAttackComponent;
class UBossOutPartPatternComponent;
class APawn_CompositeMaster;

UCLASS()
class UNREALPROJECT_API ABossOutPart : public APawn_Template
{
	GENERATED_BODY()

public:
	ABossOutPart();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Boss|OutPart|Death")
	void HandlePartDestroyed();

	UFUNCTION(BlueprintCallable, Category = "Boss|OutPart")
	void SetUseSideAttackPose(bool bEnableSideAttackPose);

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart")
	USceneComponent* GetFrontFireOrigin() const { return FrontFireOrigin; }

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart")
	USceneComponent* GetSideFireOrigin() const { return SideFireOrigin; }

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart")
	USceneComponent* GetActiveFireOrigin() const;

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart")
	UCraftAttackComponent* GetCommonAttackComponent() const { return CommonAttackComponent; }

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart")
	ULaserAttackComponent* GetLaserAttackComponent() const { return LaserAttackComponent; }

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart")
	UBossOutPartPatternComponent* GetPatternComponent() const { return PatternComponent; }

	UFUNCTION(BlueprintCallable, Category = "Boss|OutPart|Death")
	void RequestPartDestroyedSequence();

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss|OutPart|Death")
	void ReceivePartDestroyedSequenceRequested();

	UFUNCTION(BlueprintPure, Category = "Boss|OutPart")
	APawn_CompositeMaster* GetOwningCompositeMaster() const;

protected:
	UFUNCTION()
	void HandleHpChanged(float CurrentHp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> VisualRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> FrontFireOrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SideFireOrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCraftAttackComponent> CommonAttackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<ULaserAttackComponent> LaserAttackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBossOutPartPatternComponent> PatternComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart")
	FRotator DefaultVisualRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart")
	FRotator SideAttackVisualRotation = FRotator(0.0f, 0.0f, -90.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|OutPart")
	float PoseInterpSpeed = 8.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|OutPart")
	bool bUseSideAttackPose = false;

private:
	FRotator DesiredVisualRotation = FRotator::ZeroRotator;
	bool bPartDestroyedHandled = false;
};
