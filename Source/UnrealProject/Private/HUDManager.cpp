#include "HUDManager.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "..\PathFollowerComponent.h"
#include "BulletBase.h"
#include "BossOutPartPatternComponent.h"
#include "BossPartPatternCoordinatorComponent.h"
#include "BossPatternSchedulerComponent.h"
#include "BossSupportPartOrbitComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Pawn_CompositeMaster.h"
#include "PlayerRailMovementComponent.h"
#include "Snake_CompositeMaster.h"
#include "SnakeBodyChargeComponent.h"
#include "SnakeSkillManager.h"
#include "StatComponent.h"
#include "SupportFireComponent.h"
#include "TargetButtonWidget.h"

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

void AHUDManager::StartTargetMiniGame(int32 TotalTargets, UBossOutPartPatternComponent* SourcePatternComponent)
{
    if (bMiniGameActive || !TargetWidgetClass || TotalTargets <= 0)
    {
        return;
    }

    bMiniGameActive = true;
    ActiveTargetsCount = TotalTargets;
    ActiveTargetWidgets.Reset();
    ActiveMiniGameSourcePatternComponent = SourcePatternComponent;

    DestroyActiveBullets();
    SetMiniGameBattlePaused(true);

    if (MiniGameBackgroundWidgetClass)
    {
        CurrentMiniGameBackground = CreateWidget<UUserWidget>(GetWorld(), MiniGameBackgroundWidgetClass);
        if (CurrentMiniGameBackground)
        {
            CurrentMiniGameBackground->AddToViewport(100);
        }
    }

    TArray<FVector2D> SpawnedPositions;
    FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
    if (ViewportSize.X <= 0 || ViewportSize.Y <= 0)
    {
        ViewportSize = FVector2D(1920.0f, 1080.0f);
    }

    const float Margin = 200.0f;
    const float SafeDistance = 150.0f;

    for (int32 TargetIndex = 0; TargetIndex < TotalTargets; ++TargetIndex)
    {
        FVector2D BestPos(0.0f, 0.0f);

        for (int32 Retry = 0; Retry < 20; ++Retry)
        {
            const FVector2D TestPos(
                FMath::RandRange(Margin, static_cast<float>(ViewportSize.X - Margin)),
                FMath::RandRange(Margin, static_cast<float>(ViewportSize.Y - Margin)));

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
                break;
            }
        }

        UTargetButtonWidget* TargetWidget = CreateWidget<UTargetButtonWidget>(GetWorld(), TargetWidgetClass);
        if (!TargetWidget)
        {
            continue;
        }

        TargetWidget->InitializeTarget();
        TargetWidget->OnClickedEvent.AddDynamic(this, &AHUDManager::HandleTargetClicked);
        TargetWidget->AddToViewport(110);
        TargetWidget->SetTargetPosition(BestPos);

        SpawnedPositions.Add(BestPos);
        ActiveTargetWidgets.Add(TargetWidget);
    }

    if (GetWorld())
    {
        GetWorldTimerManager().ClearTimer(MiniGameFailTimerHandle);
        GetWorldTimerManager().SetTimer(
            MiniGameFailTimerHandle,
            this,
            &AHUDManager::HandleMiniGameFailed,
            FMath::Max(0.1f, MiniGameTimeLimit),
            false);
    }
}

void AHUDManager::ForceFinishMiniGame(bool bTreatAsSuccess)
{
    if (!bMiniGameActive)
    {
        return;
    }

    EndTargetMiniGame(bTreatAsSuccess);
}

bool AHUDManager::IsMiniGameOwnedByPattern(const UBossOutPartPatternComponent* SourcePatternComponent) const
{
    return bMiniGameActive && ActiveMiniGameSourcePatternComponent == SourcePatternComponent;
}

void AHUDManager::HandleTargetClicked()
{
    ActiveTargetsCount = FMath::Max(0, ActiveTargetsCount - 1);
    if (ActiveTargetsCount <= 0)
    {
        EndTargetMiniGame(true);
    }
}

void AHUDManager::EndTargetMiniGame(bool bWasSuccessful)
{
    if (!bMiniGameActive)
    {
        return;
    }

    bMiniGameActive = false;
    GetWorldTimerManager().ClearTimer(MiniGameFailTimerHandle);

    for (UTargetButtonWidget* TargetWidget : ActiveTargetWidgets)
    {
        if (IsValid(TargetWidget))
        {
            TargetWidget->RemoveFromParent();
        }
    }
    ActiveTargetWidgets.Reset();

    if (CurrentMiniGameBackground)
    {
        CurrentMiniGameBackground->RemoveFromParent();
        CurrentMiniGameBackground = nullptr;
    }

    if (bWasSuccessful)
    {
        if (IsValid(ActiveMiniGameSourcePatternComponent))
        {
            ActiveMiniGameSourcePatternComponent->DisablePatternForDuration(
                ActiveMiniGameSourcePatternComponent->GetMiniGameSuccessDisableDuration());
        }
    }
    else
    {
        ApplyMiniGameFailureDamage();
    }

    SetMiniGameBattlePaused(false);
    ActiveTargetsCount = 0;
    ActiveMiniGameSourcePatternComponent = nullptr;
}

void AHUDManager::HandleMiniGameFailed()
{
    EndTargetMiniGame(false);
}

void AHUDManager::ApplyMiniGameFailureDamage()
{
    APawn* OwningPawn = GetOwningPawn();
    if (!OwningPawn)
    {
        return;
    }

    if (UStatComponent* TargetStatComponent = OwningPawn->FindComponentByClass<UStatComponent>())
    {
        TargetStatComponent->ApplyDamage(MiniGameFailureDamage);
    }
}

void AHUDManager::SetMiniGameBattlePaused(bool bPaused)
{
    if (APawn* OwningPawn = GetOwningPawn())
    {
        OwningPawn->SetActorTickEnabled(!bPaused);

        if (UPathFollowerComponent* PathFollowerComponent = OwningPawn->FindComponentByClass<UPathFollowerComponent>())
        {
            PathFollowerComponent->SetComponentTickEnabled(!bPaused);
        }

        if (UPlayerRailMovementComponent* PlayerRailMovementComponent = OwningPawn->FindComponentByClass<UPlayerRailMovementComponent>())
        {
            PlayerRailMovementComponent->SetComponentTickEnabled(!bPaused);
        }

        if (USupportFireComponent* SupportFireComponent = OwningPawn->FindComponentByClass<USupportFireComponent>())
        {
            SupportFireComponent->SetComponentTickEnabled(!bPaused);
        }
    }

    if (!GetWorld())
    {
        return;
    }

    for (TActorIterator<APawn_CompositeMaster> CompositeIterator(GetWorld()); CompositeIterator; ++CompositeIterator)
    {
        APawn_CompositeMaster* CompositeActor = *CompositeIterator;
        if (!IsValid(CompositeActor))
        {
            continue;
        }

        if (bPaused)
        {
            if (UBossPartPatternCoordinatorComponent* BossPartPatternCoordinatorComponent = CompositeActor->GetBossPartPatternCoordinatorComponent())
            {
                BossPartPatternCoordinatorComponent->StopCurrentPattern();
            }
        }

        CompositeActor->SetActorTickEnabled(!bPaused);

        if (UBossPatternSchedulerComponent* BossPatternSchedulerComponent = CompositeActor->FindComponentByClass<UBossPatternSchedulerComponent>())
        {
            BossPatternSchedulerComponent->SetComponentTickEnabled(!bPaused);
        }

        if (UBossSupportPartOrbitComponent* BossSupportPartOrbitComponent = CompositeActor->FindComponentByClass<UBossSupportPartOrbitComponent>())
        {
            BossSupportPartOrbitComponent->SetComponentTickEnabled(!bPaused);
        }

        if (ASnake_CompositeMaster* SnakeActor = Cast<ASnake_CompositeMaster>(CompositeActor))
        {
            if (bPaused)
            {
                TArray<UChildActorComponent*> SnakeChildPartComponents;
                SnakeActor->GetChildPartComponents(SnakeChildPartComponents);

                if (USnakeSkillManager* SnakeSkillManager = SnakeActor->FindComponentByClass<USnakeSkillManager>())
                {
                    SnakeSkillManager->StopAllSkills(SnakeChildPartComponents);
                }
            }
        }

        TArray<AActor*> ChildPartActors;
        CompositeActor->GetChildPartActors(ChildPartActors);
        for (AActor* ChildPartActor : ChildPartActors)
        {
            if (!IsValid(ChildPartActor))
            {
                continue;
            }

            ChildPartActor->SetActorTickEnabled(!bPaused);

            if (UBossOutPartPatternComponent* BossOutPartPatternComponent = ChildPartActor->FindComponentByClass<UBossOutPartPatternComponent>())
            {
                if (bPaused)
                {
                    BossOutPartPatternComponent->StopActivePattern();
                }
                BossOutPartPatternComponent->SetComponentTickEnabled(!bPaused);
            }

            if (USnakeBodyChargeComponent* SnakeBodyChargeComponent = ChildPartActor->FindComponentByClass<USnakeBodyChargeComponent>())
            {
                if (bPaused)
                {
                    SnakeBodyChargeComponent->CancelSkillSequence();
                }

                SnakeBodyChargeComponent->SetComponentTickEnabled(!bPaused);
            }
        }
    }
}

void AHUDManager::DestroyActiveBullets()
{
    if (!GetWorld())
    {
        return;
    }

    TArray<ABulletBase*> ActiveBullets;
    for (TActorIterator<ABulletBase> BulletIterator(GetWorld()); BulletIterator; ++BulletIterator)
    {
        if (ABulletBase* BulletActor = *BulletIterator)
        {
            ActiveBullets.Add(BulletActor);
        }
    }

    for (ABulletBase* BulletActor : ActiveBullets)
    {
        if (IsValid(BulletActor))
        {
            BulletActor->Destroy();
        }
    }
}
