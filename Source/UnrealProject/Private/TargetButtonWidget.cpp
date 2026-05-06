#include "TargetButtonWidget.h"
#include "Components/Button.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"

void UTargetButtonWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (TargetButton)
    {
        TargetButton->OnClicked.AddDynamic(this, &UTargetButtonWidget::OnButtonClicked);
    }

    // 처음에 랜덤 위치로 이동
    MoveToRandomPosition();
}

void UTargetButtonWidget::InitializeTarget()
{
    // 단일 타겟으로 변경되어 특별한 초기화 불필요
}

void UTargetButtonWidget::SetTargetPosition(FVector2D NewPos)
{
    // Viewport 위치 설정
    SetPositionInViewport(NewPos);

    // CanvasSlot 위치 설정
    UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
    if (CanvasSlot)
    {
        CanvasSlot->SetPosition(NewPos);
    }
}

void UTargetButtonWidget::OnButtonClicked()
{
    OnClickedEvent.Broadcast();
    RemoveFromParent();
}

void UTargetButtonWidget::MoveToRandomPosition()
{
    FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
    
    if (ViewportSize.X <= 0 || ViewportSize.Y <= 0)
    {
        // 뷰포트 사이즈를 가져오지 못한 경우 기본값 설정 (보통 첫 프레임)
        ViewportSize = FVector2D(1920.f, 1080.f);
    }

    float Margin = 200.0f;
    float RandomX = FMath::RandRange(Margin, (float)(ViewportSize.X - Margin));
    float RandomY = FMath::RandRange(Margin, (float)(ViewportSize.Y - Margin));

    SetPositionInViewport(FVector2D(RandomX, RandomY));

    UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
    if (CanvasSlot)
    {
        CanvasSlot->SetPosition(FVector2D(RandomX, RandomY));
    }
}
