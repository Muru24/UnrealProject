#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StatComponent.h"
#include "ChaosDestructiblePart.generated.h"

class UGeometryCollectionComponent;
class UBoxComponent;

// 파트가 완전히 파괴됐을 때 발생하는 델리게이트 (보스 패턴 연동용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPartFullyDestroyed, AChaosDestructiblePart*, DestroyedPart);

UCLASS()
class UNREALPROJECT_API AChaosDestructiblePart : public AActor
{
	GENERATED_BODY()

public:
	AChaosDestructiblePart();

protected:
	virtual void BeginPlay() override;

public:
	// === 컴포넌트 ===
	// 카오스 물리 메시 (블루프린트에서 Geometry Collection 할당)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGeometryCollectionComponent> GeomCollection;

	// 총알 히트 감지용 콜리전 박스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> HitCollider;

	// 총알 데미지 시스템과 연동되는 스탯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStatComponent> StatComp;

	// === 체력 설정 ===
	// 이 값이 HP. 총알 1발 데미지(기본 1.0) 기준으로 몇 발에 파괴될지 결정됨
	// 예) InitialHP=10 → 총알 10발에 파괴
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destruction|Health")
	float InitialHP = 10.0f;

	// === 이벤트 ===
	UPROPERTY(BlueprintAssignable, Category = "Destruction|Events")
	FOnPartFullyDestroyed OnPartFullyDestroyed;

	// === Blueprint Callable 함수 ===

	// 외부에서 강제로 앵커를 해제하여 물리 활성화 (보스 패턴에서 호출 가능)
	UFUNCTION(BlueprintCallable, Category = "Destruction")
	void Unanchor();

private:
	UFUNCTION()
	void OnHealthChanged(float CurrentHp);
	bool bIsDestroyed = false;
};
