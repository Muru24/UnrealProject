#include "P_Player.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LockOnComponent.h"
#include "Path.h"
#include "PathFollowerComponent.h"
#include "PlayerAimFireComponent.h"
#include "PlayerCameraRigComponent.h"
#include "PlayerRailMovementComponent.h"
#include "RailOffsetComponent.h"
#include "SquadCraftActor.h"
#include "SquadComponent.h"
#include "SquadRuntimeComponent.h"
#include "StatComponent.h"
#include "SupportFireComponent.h"
#include "InputCoreTypes.h"

AP_Player::AP_Player()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.0f;
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
	PlayerCameraRigComponent = CreateDefaultSubobject<UPlayerCameraRigComponent>(TEXT("PlayerCameraRigComp"));
	PlayerRailMovementComponent = CreateDefaultSubobject<UPlayerRailMovementComponent>(TEXT("PlayerRailMovementComp"));
	SupportFireComponent = CreateDefaultSubobject<USupportFireComponent>(TEXT("SupportFireComp"));
}

void AP_Player::BeginPlay()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetInputMode(FInputModeGameAndUI());
	}

	APawn_Template::BeginPlay();

	if (PathFollower && !PathFollower->HasValidBaseTransform())
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

	if (SquadRuntimeComponent)
	{
		SquadRuntimeComponent->SpawnCrafts(this, SquadComponent, PlaneMesh);
		SquadRuntimeComponent->RefreshCraftStates(SquadComponent);
	}

	if (PlayerCameraRigComponent)
	{
		PlayerCameraRigComponent->UpdateCameraAnchor(SpringArm, GetActiveCraft(), true);
	}
}

void AP_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &APawn_Template::Fire);
	PlayerInputComponent->BindAction(TEXT("LockOn"), IE_Pressed, LockOn.Get(), &ULockOnComponent::TraceTarget);
	PlayerInputComponent->BindAction(TEXT("TargetChange"), IE_Pressed, LockOn.Get(), &ULockOnComponent::ChangeTarget);
	PlayerInputComponent->BindAction(TEXT("SwapLeft"), IE_Pressed, this, &AP_Player::SwapSquadLeft);
	PlayerInputComponent->BindAction(TEXT("SwapRight"), IE_Pressed, this, &AP_Player::SwapSquadRight);
	PlayerInputComponent->BindAction(TEXT("Acceleration"), IE_Pressed, this, &AP_Player::Accelerator);
	PlayerInputComponent->BindAxis(TEXT("MoveHorizontal"), this, &AP_Player::MoveHorizontal);
	PlayerInputComponent->BindAxis(TEXT("MoveVertical"), this, &AP_Player::MoveVertical);
	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &AP_Player::TriggerOffensiveSkill);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AP_Player::TriggerBuffSkill);
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
	}
	HandleSupportAutoFire();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (PlayerCameraRigComponent)
		{
			PlayerCameraRigComponent->UpdateCameraPan(PlayerController, SpringArm, DeltaTime);
		}
	}
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
		SquadComponent->ShiftActiveSlotLeft();
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
		SquadComponent->ShiftActiveSlotRight();
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

void AP_Player::TriggerOffensiveSkill()
{
	if (OffensiveSkillRemainingUses <= 0)
	{
		return;
	}

	ASquadCraftActor* ActiveCraft = GetActiveCraft();
	if (!ActiveCraft)
	{
		return;
	}

	AActor* SkillTarget = LockOn ? LockOn->GetCurrentTarget() : nullptr;
	if (ActiveCraft->TryActivateOffensiveSkill(SkillTarget))
	{
		OffensiveSkillRemainingUses--;
	}
}

void AP_Player::TriggerBuffSkill()
{
	if (BuffSkillRemainingUses <= 0)
	{
		return;
	}

	ASquadCraftActor* ActiveCraft = GetActiveCraft();
	if (!ActiveCraft)
	{
		return;
	}

	if (ActiveCraft->TryActivateBuffSkill(nullptr))
	{
		BuffSkillRemainingUses--;
	}
}

void AP_Player::HandleSupportAutoFire()
{
	if (!SupportFireComponent || !SquadRuntimeComponent)
	{
		return;
	}

	TArray<ASquadCraftActor*> SquadCrafts;
	SquadRuntimeComponent->GetAllCrafts(SquadCrafts);
	SupportFireComponent->HandleSupportAutoFire(SquadCrafts, LockOn, this);
}

ASquadCraftActor* AP_Player::GetActiveCraft() const
{
	return SquadRuntimeComponent ? SquadRuntimeComponent->GetActiveCraft(SquadComponent) : nullptr;
}
