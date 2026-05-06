#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetSuccess);

/**
 * 화면상의 랜덤한 위치에 나타나는 표적 버튼 위젯입니다.
 */
UCLASS()
class UNREALPROJECT_API UTargetButtonWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 미니게임 초기화 (생략 가능) */
    UFUNCTION(BlueprintCallable, Category = "TargetGame")
    void InitializeTarget();

    /** 특정 위치로 타겟 이동 */
    UFUNCTION(BlueprintCallable, Category = "TargetGame")
    void SetTargetPosition(FVector2D NewPos);

    /** 클릭 시 호출될 이벤트 */
    UPROPERTY(BlueprintAssignable, Category = "TargetGame")
    FOnTargetSuccess OnClickedEvent;

protected:
    virtual void NativeConstruct() override;

    /** Blueprint에서 바인딩할 버튼 */
    UPROPERTY(meta = (BindWidget))
    class UButton* TargetButton;

    /** 버튼 클릭 이벤트 핸들러 */
    UFUNCTION()
    void OnButtonClicked();

    /** 버튼을 화면 내 랜덤한 위치로 이동 */
    void MoveToRandomPosition();

private:
    // 각 위젯은 이제 단일 타겟이므로 RemainingClicks가 필요 없음
};
