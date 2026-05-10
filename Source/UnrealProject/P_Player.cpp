#include "P_Player.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LockOnComponent.h"
#include "Path.h"
#include "PathFollowerComponent.h"
#include "PlayerAimFireComponent.h"
#include "PlayerCameraFeedbackComponent.h"
#include "PlayerCameraRigComponent.h"
#include "PlayerRailMovementComponent.h"
#include "RailOffsetComponent.h"
#include "SquadCraftActor.h"
#include "SquadComponent.h"
#include "SquadRuntimeComponent.h"
#include "StatComponent.h"
#include "SupportFireComponent.h"
#include "HUDManager.h"
#include "InputCoreTypes.h"
#include "Components/WidgetComponent.h"

AP_Player::AP_Player()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 700.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 8.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 10.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	LockOn = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOn"));

	PathFollower = CreateDefaultSubobject<UPathFollowerComponent>(TEXT("PathFollowerComp"));
	PathFollower->SetApplyOwnerTransform(false);

	RailOffset = CreateDefaultSubobject<URailOffsetComponent>(TEXT("RailOffsetComp"));
	SquadComponent = CreateDefaultSubobject<USquadComponent>(TEXT("SquadComp"));
	SquadRuntimeComponent = CreateDefaultSubobject<USquadRuntimeComponent>(TEXT("SquadRuntimeComp"));
	PlayerAimFireComponent = CreateDefaultSubobject<UPlayerAimFireComponent>(TEXT("PlayerAimFireComp"));
	PlayerCameraFeedbackComponent = CreateDefaultSubobject<UPlayerCameraFeedbackComponent>(TEXT("PlayerCameraFeedbackComp"));
	PlayerCameraRigComponent = CreateDefaultSubobject<UPlayerCameraRigComponent>(TEXT("PlayerCameraRigComp"));
	PlayerRailMovementComponent = CreateDefaultSubobject<UPlayerRailMovementComponent>(TEXT("PlayerRailMovementComp"));
	SupportFireComponent = CreateDefaultSubobject<USupportFireComponent>(TEXT("SupportFireComp"));

	LeftFloatingUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("LeftFloatingUI"));
	LeftFloatingUI->SetupAttachment(RootComponent);
	LeftFloatingUI->SetVisibility(false);
}

void AP_Player::BeginPlay()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetInputMode(FInputModeGameAndUI());
	}

	APawn_Template::BeginPlay();

	if (PathFollower && !IsValid(PathFollower->GetTargetPathActor()))
	{
		if (AActor* PathActor = UGameplayStatics::GetActorOfClass(GetWorld(), APath::StaticClass()))
		{
			PathFollower->SetTargetPath(PathActor);
		}
	}

	if (PlaneMesh)
	{
		PlaneMesh->SetHiddenInGame(true);
		PlaneMesh->SetVisibility(false);
	}

	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (SquadRuntimeComponent)
	{
		SquadRuntimeComponent->SpawnCrafts(this, SquadComponent, PlaneMesh);
		SquadRuntimeComponent->RefreshCraftStates(SquadComponent);

		TArray<ASquadCraftActor*> AllCrafts;
		SquadRuntimeComponent->GetAllCrafts(AllCrafts);
		for (ASquadCraftActor* Craft : AllCrafts)
		{
			if (Craft)
			{
				Craft->OnCraftDefeated.AddUObject(this, &AP_Player::HandleSquadCraftDefeated);
			}
		}
	}

	if (PlayerCameraRigComponent)
	{
		PlayerCameraRigComponent->UpdateCameraAnchor(SpringArm, GetActiveCraft(), true);
	}


}

void AP_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("LockOn"), IE_Pressed, LockOn.Get(), &ULockOnComponent::TraceTarget);
	PlayerInputComponent->BindAction(TEXT("TargetChange"), IE_Pressed, LockOn.Get(), &ULockOnComponent::ChangeTarget);
	PlayerInputComponent->BindAction(TEXT("SwapLeft"), IE_Pressed, this, &AP_Player::SwapSquadLeft);
	PlayerInputComponent->BindAction(TEXT("SwapRight"), IE_Pressed, this, &AP_Player::SwapSquadRight);
	PlayerInputComponent->BindAction(TEXT("Acceleration"), IE_Pressed, this, &AP_Player::Accelerator);
	PlayerInputComponent->BindAxis(TEXT("MoveHorizontal"), this, &AP_Player::MoveHorizontal);
	PlayerInputComponent->BindAxis(TEXT("MoveVertical"), this, &AP_Player::MoveVertical);
	PlayerInputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &AP_Player::ToggleSquadAutoAttack);
	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &AP_Player::TriggerOffensiveSkill);
}

void AP_Player::Accelerator()
{
	if (PathFollower && StatComponent)
	{
		PathFollower->SetAcceleration(StatComponent->GetAcceleration());
	}
}

void AP_Player::Fire()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerAimFireComponent || !PlayerController)
	{
		return;
	}

	PlayerAimFireComponent->FireActiveCraft(PlayerController, LockOn, GetActiveCraft(), this);
}

void AP_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerRailMovementComponent)
	{
		PlayerRailMovementComponent->ApplyRailMovement(this, PathFollower, RailOffset, SquadComponent, SquadRuntimeComponent, DeltaTime);
	}
	if (PlayerCameraRigComponent)
	{
		PlayerCameraRigComponent->UpdateCameraAnchor(SpringArm, GetActiveCraft(), false);
		PlayerCameraRigComponent->UpdateCameraZoom(SpringArm, PathFollower && PathFollower->IsAccelerationActive(), DeltaTime);
		PlayerCameraRigComponent->UpdateCameraShake(SpringArm, DeltaTime);
	}
	HandleActiveAutoFire();
	HandleSupportAutoFire();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (PlayerCameraRigComponent)
		{
			const bool bSuppressCameraPan = RailOffset && !RailOffset->GetCurrentInput().IsNearlyZero();
			PlayerCameraRigComponent->UpdateCameraPan(PlayerController, SpringArm, DeltaTime, bSuppressCameraPan);
		}
	}

	if (ASquadCraftActor* ActiveCraft = GetActiveCraft())
	{
		const FVector CraftLoc = ActiveCraft->GetActorLocation();
		const FVector CraftForward = ActiveCraft->GetActorForwardVector();
		const FVector CraftRight = ActiveCraft->GetActorRightVector();
		const FRotator CraftRot = ActiveCraft->GetActorRotation();
		const FQuat CraftQuat = CraftRot.Quaternion();

		if (LeftFloatingUI && LeftFloatingUI->IsVisible())
		{
			const FVector LeftPos = CraftLoc + CraftQuat.RotateVector(LeftUIOffset);
			LeftFloatingUI->SetWorldLocation(LeftPos);
			LeftFloatingUI->SetWorldRotation(CraftRot + LeftUIRotation);
		}
	}

	// 버프 스킬 자동 발동 처리 (스킬 자체 쿨다운에 의존)
	UpdateAutoBuffSkill(DeltaTime);
}

void AP_Player::MoveHorizontal(float Value)
{
	if (RailOffset)
	{
		RailOffset->SetHorizontalInput(Value);
	}
}

void AP_Player::MoveVertical(float Value)
{
	if (RailOffset)
	{
		RailOffset->SetVerticalInput(Value);
	}
}

void AP_Player::SwapSquadLeft()
{
	if (SquadComponent)
	{
		for (int32 Attempt = 0; Attempt < 3; ++Attempt)
		{
			SquadComponent->ShiftActiveSlotLeft();
			if (GetActiveCraft())
			{
				break;
			}
		}
	}

	if (SquadRuntimeComponent)
	{
		SquadRuntimeComponent->RefreshCraftStates(SquadComponent);
	}

	if (PlayerCameraRigComponent)
	{
		PlayerCameraRigComponent->UpdateCameraAnchor(SpringArm, GetActiveCraft(), false);
	}
}

void AP_Player::SwapSquadRight()
{
	if (SquadComponent)
	{
		for (int32 Attempt = 0; Attempt < 3; ++Attempt)
		{
			SquadComponent->ShiftActiveSlotRight();
			if (GetActiveCraft())
			{
				break;
			}
		}
	}

	if (SquadRuntimeComponent)
	{
		SquadRuntimeComponent->RefreshCraftStates(SquadComponent);
	}

	if (PlayerCameraRigComponent)
	{
		PlayerCameraRigComponent->UpdateCameraAnchor(SpringArm, GetActiveCraft(), false);
	}
}

void AP_Player::ToggleSquadAutoAttack()
{
	bSquadAutoAttackEnabled = !bSquadAutoAttackEnabled;
}

void AP_Player::TriggerOffensiveSkill()
{
	ASquadCraftActor* ActiveCraft = GetActiveCraft();
	if (!ActiveCraft)
	{
		return;
	}

	AActor* SkillTarget = LockOn ? LockOn->GetCurrentTarget() : nullptr;
	ActiveCraft->TryActivateOffensiveSkill(SkillTarget);
}

void AP_Player::TriggerBuffSkill()
{
	if (!SquadRuntimeComponent)
	{
		return;
	}

	TArray<ASquadCraftActor*> AllCrafts;
	SquadRuntimeComponent->GetAllCrafts(AllCrafts);

	for (ASquadCraftActor* Craft : AllCrafts)
	{
		if (Craft)
		{
			Craft->TryActivateBuffSkill(nullptr);
		}
	}
}

void AP_Player::UpdateAutoBuffSkill(float DeltaTime)
{
	if (BuffAutoActivateInterval <= 0.0f)
	{
		TriggerBuffSkill();
		return;
	}

	BuffAutoActivateTimer += DeltaTime;
	if (BuffAutoActivateTimer < BuffAutoActivateInterval)
	{
		return;
	}

	BuffAutoActivateTimer = 0.0f;
	TriggerBuffSkill();
}

void AP_Player::HandleSupportAutoFire()
{
	if (!bSquadAutoAttackEnabled || !SupportFireComponent || !SquadRuntimeComponent)
	{
		return;
	}

	TArray<ASquadCraftActor*> SquadCrafts;
	SquadRuntimeComponent->GetAllCrafts(SquadCrafts);
	SupportFireComponent->HandleSupportAutoFire(SquadCrafts, LockOn, this);
}

void AP_Player::HandleActiveAutoFire()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!bSquadAutoAttackEnabled || !PlayerAimFireComponent || !PlayerController)
	{
		return;
	}

	PlayerAimFireComponent->TryAutoFireActiveCraft(PlayerController, LockOn, GetActiveCraft(), this);
}

ASquadCraftActor* AP_Player::GetActiveCraft() const
{
	return SquadRuntimeComponent ? SquadRuntimeComponent->GetActiveCraft(SquadComponent) : nullptr;
}

void AP_Player::HandleSquadCraftDefeated(ASquadCraftActor* DefeatedCraft)
{
	if (!SquadRuntimeComponent || !SquadComponent || !DefeatedCraft)
	{
		return;
	}

	if (!GetActiveCraft())
	{
		SquadRuntimeComponent->SelectFirstOperationalCraft(SquadComponent);
	}

	SquadRuntimeComponent->RefreshCraftStates(SquadComponent);

	if (PlayerCameraRigComponent)
	{
		PlayerCameraRigComponent->UpdateCameraAnchor(SpringArm, GetActiveCraft(), false);
	}

	if (PlayerCameraFeedbackComponent)
	{
		PlayerCameraFeedbackComponent->PlayCraftDestroyedShake(this);
	}

	if (!SquadRuntimeComponent->HasOperationalCrafts() && StatComponent)
	{
		StatComponent->ApplyDamage(999999.0f);
	}
}

void AP_Player::StartTestMiniGame()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (AHUDManager* HUD = Cast<AHUDManager>(PC->GetHUD()))
		{
			// 테스트를 위해 10개의 타겟을 생성하는 미니게임 실행
			HUD->StartTargetMiniGame(10);
		}
	}
}
