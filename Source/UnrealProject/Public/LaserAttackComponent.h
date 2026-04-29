#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LaserAttackComponent.generated.h"

class ABeamEffectActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPROJECT_API ULaserAttackComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULaserAttackComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "Laser")
    void StartLaser(AActor* InTarget);

    UFUNCTION(BlueprintCallable, Category = "Laser")
    void StopLaser();

    // 빔의 발사 지점(SceneComponent)을 설정합니다. 없을 경우 Actor 위치를 사용합니다.
    void SetFireOrigin(USceneComponent* InOrigin) { FireOrigin = InOrigin; }
    AActor* GetTargetActor() const { return TargetActor; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
    TSubclassOf<ABeamEffectActor> BeamActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
    float AttackActiveDuration = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
    float InitialBeamSize = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
    float FinalBeamSize = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Config")
    float BeamExtraDistance = 500.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Laser|Runtime")
    ABeamEffectActor* ActiveBeamActor;

    UPROPERTY(BlueprintReadOnly, Category = "Laser|Runtime")
    bool bAttackActive = false;

    UPROPERTY(BlueprintReadOnly, Category = "Laser|Runtime")
    float AttackTimeTracker = 0.0f;

    UPROPERTY()
    AActor* TargetActor;

    UPROPERTY()
    USceneComponent* FireOrigin;

    FTimerHandle AttackStopTimerHandle;

    void FireBeam();
    void DeactivateActiveBeam();
    FVector GetTargetLocationWithExtension() const;
    FVector GetFireOriginLocation() const;
};
