#pragma once

#include "CoreMinimal.h"
#include "Pawn_CompositeMaster.h"
#include "Snake_CompositeMaster.generated.h"

class USnakePathController;
class USnakeSkillManager;
class USnakeBodyChargeComponent;
class UChildActorComponent;

UCLASS()
class UNREALPROJECT_API ASnake_CompositeMaster : public APawn_CompositeMaster
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USnakePathController> PathController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USnakeBodyChargeComponent> HeadSkillComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USnakeSkillManager> SkillManager;

public:
	ASnake_CompositeMaster();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void GetOrbitTransform(
		float Angle,
		const FRotator& Rotation,
		FVector& OutLocation,
		FVector& OutForward,
		FVector& OutRight,
		FVector& OutUp,
		const FVector& CenterLoc);

	UPROPERTY(EditAnywhere, Category = "Snake|Orbit")
	TObjectPtr<AActor> CenterActor;

	UPROPERTY(EditAnywhere, Category = "Snake|Orbit")
	float OrbitRadius = 900.0f;

	UPROPERTY(EditAnywhere, Category = "Snake|Orbit")
	float OrbitSpeed = 0.9f;

	UPROPERTY(EditAnywhere, Category = "Snake|Orbit")
	float SegmentAngleGap = 0.35f;

	UPROPERTY(EditAnywhere, Category = "Snake|Spiral")
	bool RunPatten = false;

	UPROPERTY(EditAnywhere, Category = "Snake|Spiral")
	float ShrinkSpeed = 3.0f;

	TArray<float> SegAngleOffsets;
	float CurrentOrbitAngle = 0.0f;

	void UnregisterSegment(UChildActorComponent* SegmentComp);
};
