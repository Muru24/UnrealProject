#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeakPointTarget.generated.h"

class USphereComponent;
class UWidgetComponent;
class UStatComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetDestroyed, AWeakPointTarget*, DestroyedTarget);

UCLASS()
class UNREALPROJECT_API AWeakPointTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeakPointTarget();

protected:
	virtual void BeginPlay() override;

public:
	// === 컴포넌트 ===
	
	// 총알 충돌을 감지할 투명한 구체 (이 영역에 맞으면 데미지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> HitSphere;

	// 과녁 이미지나 체력바를 띄울 UI 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> TargetWidget;

	// 체력 및 피격 횟수를 관리하는 스탯 컴포넌트 (총알과 자동 연동됨)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStatComponent> StatComp;

	// === 설정 변수 ===
	
	// 과녁의 초기 체력 (총알 1발당 1의 데미지라면 이 값이 곧 타격 횟수)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Settings")
	float InitialHP = 5.0f;

	// === 이벤트 ===
	
	// 체력이 0이 되어 과녁이 파괴될 때 발생하는 델리게이트 (보스 무력화 패턴 연동용)
	UPROPERTY(BlueprintAssignable, Category = "Target Events")
	FOnTargetDestroyed OnTargetDestroyed;

	// === 블루프린트 연동 함수 ===
	
	// UI에서 체력 비율(0.0 ~ 1.0)을 그리기 위해 호출할 수 있는 함수
	UFUNCTION(BlueprintPure, Category = "Target Info")
	float GetHealthPercent() const;

private:
	// 스탯 컴포넌트의 HP 변경 이벤트 수신 함수
	UFUNCTION()
	void OnHealthChanged(float CurrentHp);

	bool bIsDestroyed = false;
};
