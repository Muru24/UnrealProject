#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeamSpawner.generated.h"

class ABeamObjcet;
class UChildActorComponent;

UCLASS()
class UNREALPROJECT_API ABeamSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ABeamSpawner();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Config")
	TSubclassOf<ABeamObjcet> BeamClass;

	// 생성할 빔의 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Config", meta = (ClampMin = "1"))
	int32 BeamCount = 3;

	// 빔 사이의 간격 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Config")
	float BeamSpacing = 300.0f;

	// 배치 방향 (0=Y축, 1=Z축)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Config")
	bool bAlignOnZAxis = false;

	// true 시 중앙 빔을 기준으로 양쪽으로 정렬
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Config")
	bool bCenterAlignment = true;

private:
	UPROPERTY()
	TArray<UChildActorComponent*> BeamComponents;

	void ClearBeams();
	void CreateBeams();
};
