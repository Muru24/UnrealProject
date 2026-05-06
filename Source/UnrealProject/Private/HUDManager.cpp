#include "HUDManager.h"
#include "Blueprint/UserWidget.h"
#include "TargetButtonWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void AHUDManager::BeginPlay()
{
    Super::BeginPlay();

    if (HUDWidgetClass)
    {
        CurrentHUD = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
        if (CurrentHUD)
        {
            CurrentHUD->AddToViewport();
        }
    }
}

void AHUDManager::StartTargetMiniGame(int32 TotalTargets)
{
    if (TargetWidgetClass)
    {
        ActiveTargetsCount = TotalTargets;
        TArray<FVector2D> SpawnedPositions;
        
        FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
        if (ViewportSize.X <= 0 || ViewportSize.Y <= 0) ViewportSize = FVector2D(1920.f, 1080.f);

        float Margin = 200.0f;
        float SafeDistance = 150.0f; 

        for (int32 i = 0; i < TotalTargets; ++i)
        {
            FVector2D BestPos(0, 0);
            bool bFoundValidPos = false;

            for (int32 Retry = 0; Retry < 20; ++Retry)
            {
                FVector2D TestPos(
                    FMath::RandRange(Margin, (float)(ViewportSize.X - Margin)),
                    FMath::RandRange(Margin, (float)(ViewportSize.Y - Margin))
                );

                bool bOverlaps = false;
                for (const FVector2D& PrevPos : SpawnedPositions)
                {
                    if (FVector2D::Distance(TestPos, PrevPos) < SafeDistance)
                    {
                        bOverlaps = true;
                        break;
                    }
                }

                if (!bOverlaps)
                {
                    BestPos = TestPos;
                    bFoundValidPos = true;
                    break;
                }
            }

            UTargetButtonWidget* TargetWidget = CreateWidget<UTargetButtonWidget>(GetWorld(), TargetWidgetClass);
            if (TargetWidget)
            {
                TargetWidget->InitializeTarget();
                TargetWidget->OnClickedEvent.AddDynamic(this, &AHUDManager::HandleTargetClicked);
                TargetWidget->AddToViewport();
                
                TargetWidget->SetTargetPosition(BestPos);
                SpawnedPositions.Add(BestPos);
            }
        }
    }
}

void AHUDManager::HandleTargetClicked()
{
    ActiveTargetsCount--;
}