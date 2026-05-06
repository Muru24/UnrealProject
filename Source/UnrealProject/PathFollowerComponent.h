#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PathFollowerComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UPathFollowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPathFollowerComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetTargetPath(AActor* InPathActor) { TargetPathActor = InPathActor; }
	AActor* GetTargetPathActor() const { return TargetPathActor; }
	void SetPathSpeed(float Speed) { MoveSpeed = Speed; }
	void SetApplyOwnerTransform(bool bInApplyOwnerTransform) { bApplyOwnerTransform = bInApplyOwnerTransform; }
	void SetAcceleration(float Value);
	bool IsAccelerationActive() const { return Acceleration > 0.0f && RemainingAccelerationTime > 0.0f; }
	bool HasValidBaseTransform() const { return bHasValidBaseTransform; }
	const FTransform& GetBaseWorldTransform() const { return BaseWorldTransform; }

	float GetCurrentDistance() const { return CurrentDistance; }

protected:
	UPROPERTY(EditAnywhere, Category = "Movement")
	TObjectPtr<AActor> TargetPathActor;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Acceleration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AccelerationDuration = 0.25f;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	float LocationInterpSpeed = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float BankingIntensity = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RotationInterpSpeed = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Movement|Banking")
	float BankingInterpSpeed = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Movement|Banking")
	float PitchExaggeration = 1.2f;

	UPROPERTY(EditAnywhere, Category = "Movement|Banking")
	float CurrentDistance = 0.0f;

	float InterpolatedYawDelta = 0.0f;
	float CurrentRoll = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bApplyOwnerTransform = true;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	FTransform BaseWorldTransform;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	float RemainingAccelerationTime = 0.0f;

	bool bHasValidBaseTransform = false;
};
