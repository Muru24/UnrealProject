#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossSupportPartOrbitComponent.generated.h"

class APawn_CompositeMaster;
class UChildActorComponent;

USTRUCT()
struct FBossSupportPartOrbitState
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UChildActorComponent> PartComponent;

	UPROPERTY()
	float AngleOffsetRadians = 0.0f;

	UPROPERTY()
	float OrbitRadius = 0.0f;

	UPROPERTY()
	float HeightOffset = 0.0f;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API UBossSupportPartOrbitComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossSupportPartOrbitComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Boss|Orbit")
	void RefreshOrbitParts();

	UFUNCTION(BlueprintCallable, Category = "Boss|Orbit")
	void SetOrbitEnabled(bool bEnabled) { bOrbitEnabled = bEnabled; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Orbit")
	bool bOrbitEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Orbit")
	float OrbitSpeedDegrees = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Orbit")
	float SpeedBurstInterval = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Orbit")
	float SpeedBurstDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Orbit")
	float SpeedBurstMultiplier = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Orbit")
	bool bAutoRefreshOnBeginPlay = true;

private:
	UPROPERTY()
	TObjectPtr<APawn_CompositeMaster> OwnerCompositeMaster;

	UPROPERTY()
	TArray<FBossSupportPartOrbitState> OrbitPartStates;

	float CurrentOrbitAngleRadians = 0.0f;
	float SpeedBurstIntervalTimer = 0.0f;
	float SpeedBurstDurationTimer = 0.0f;
	bool bSpeedBurstActive = false;

	void UpdateOrbit(float DeltaTime);
};
