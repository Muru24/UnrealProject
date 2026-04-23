#include "P_Player.h"

#include "Camera/CameraComponent.h"
#include "EnemyManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LockOnComponent.h"
#include "Path.h"
#include "PathFollowerComponent.h"
#include "RailOffsetComponent.h"
#include "SquadCraftActor.h"

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

	SpawnSquadCrafts();
	RefreshSquadCrafts();
}

void AP_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &APawn_Template::Fire);
	PlayerInputComponent->BindAction(TEXT("LockOn"), IE_Pressed, LockOn, &ULockOnComponent::TraceTarget);
	PlayerInputComponent->BindAction(TEXT("TargetChange"), IE_Pressed, LockOn, &ULockOnComponent::ChangeTarget);
	PlayerInputComponent->BindAction(TEXT("SwapLeft"), IE_Pressed, this, &AP_Player::SwapSquadLeft);
	PlayerInputComponent->BindAction(TEXT("SwapRight"), IE_Pressed, this, &AP_Player::SwapSquadRight);
	PlayerInputComponent->BindAxis(TEXT("MoveHorizontal"), this, &AP_Player::MoveHorizontal);
	PlayerInputComponent->BindAxis(TEXT("MoveVertical"), this, &AP_Player::MoveVertical);
}

void AP_Player::Fire()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController || !LockOn)
	{
		return;
	}

	FVector TargetPoint = FVector::ZeroVector;
	AActor* CurrentTarget = nullptr;

	if (LockOn->isLockOn && LockOn->target)
	{
		TargetPoint = LockOn->target->GetActorLocation();
		CurrentTarget = LockOn->target;
	}
	else
	{
		FVector MouseLocation = FVector::ZeroVector;
		FVector MouseDirection = FVector::ZeroVector;
		if (PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
		{
			const FVector TraceStart = MouseLocation;
			const FVector TraceEnd = TraceStart + (MouseDirection * 10000.0f);

			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);

			if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
			{
				TargetPoint = HitResult.Location;
			}
			else
			{
				TargetPoint = TraceEnd;
			}
		}
	}

	if (ASquadCraftActor* ActiveCraft = GetActiveCraft())
	{
		ActiveCraft->FireAt(TargetPoint, CurrentTarget, this);
	}
}

void AP_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyRailMovement(DeltaTime);
	HandleSupportAutoFire();
	UpdateCameraPan(DeltaTime);
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
		RefreshSquadCrafts();
	}
}

void AP_Player::SwapSquadRight()
{
	if (SquadComponent)
	{
		SquadComponent->ShiftActiveSlotRight();
		RefreshSquadCrafts();
	}
}

void AP_Player::ApplyRailMovement(float DeltaTime)
{
	if (!PathFollower || !RailOffset || !PathFollower->HasValidBaseTransform())
	{
		return;
	}

	RailOffset->UpdateOffset(DeltaTime);

	const FTransform& BaseWorldTransform = PathFollower->GetBaseWorldTransform();
	const FVector2D LocalOffset = RailOffset->GetCurrentOffset();
	const FRotator VisualTiltRotation = RailOffset->GetVisualTiltRotation();

	const FVector FinalLocation =
		BaseWorldTransform.GetLocation()
		+ (BaseWorldTransform.GetUnitAxis(EAxis::Y) * LocalOffset.X)
		+ (BaseWorldTransform.GetUnitAxis(EAxis::Z) * LocalOffset.Y);

	const FRotator BaseRotation = BaseWorldTransform.Rotator();
	SetActorLocationAndRotation(FinalLocation, BaseRotation);

	RefreshSquadCrafts();

	if (ASquadCraftActor* ActiveCraft = GetActiveCraft())
	{
		ActiveCraft->SetDesiredRelativeTransform(
			SquadComponent ? SquadComponent->GetSlotOffset(SquadComponent->GetActiveSlot()) : FVector::ZeroVector,
			FRotator(VisualTiltRotation.Pitch, 0.0f, VisualTiltRotation.Roll));
	}
}

void AP_Player::UpdateCameraPan(float DeltaTime)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController || !SpringArm)
	{
		return;
	}

	int32 ViewportSizeX = 0;
	int32 ViewportSizeY = 0;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (PlayerController->GetMousePosition(MouseX, MouseY))
	{
		float RangeX = (MouseX / (ViewportSizeX * 0.5f)) - 1.0f;
		float RangeY = (MouseY / (ViewportSizeY * 0.5f)) - 1.0f;

		if (FMath::Abs(RangeX) < MouseDeadZone)
		{
			RangeX = 0.0f;
		}

		if (RangeY > 0.0f)
		{
			RangeY = 0.0f;
		}

		RangeX = FMath::Clamp(RangeX, -1.0f, 1.0f);
		RangeY = FMath::Clamp(RangeY, -1.0f, 1.0f);

		const float MaxRotationAngle = 12.0f;
		const FRotator TargetRotation(-RangeY * MaxRotationAngle, RangeX * MaxRotationAngle, 0.0f);

		SpringArm->SetRelativeRotation(
			FMath::RInterpTo(SpringArm->GetRelativeRotation(), TargetRotation, DeltaTime, CameraMoveSpeed));
	}
}

void AP_Player::HandleSupportAutoFire()
{
	if (!SquadComponent)
	{
		return;
	}

	AActor* PreferredTarget = GetPreferredAutoFireTarget();
	if (!PreferredTarget)
	{
		return;
	}

	const FVector TargetPoint = PreferredTarget->GetActorLocation();

	for (ESquadSlot Slot : SquadComponent->GetAllSlots())
	{
		ASquadCraftActor* Craft = GetCraftForSlot(Slot);
		if (!Craft)
		{
			continue;
		}

		Craft->TryAutoFireAt(TargetPoint, PreferredTarget, this);
	}
}

void AP_Player::SpawnSquadCrafts()
{
	if (!GetWorld() || !SquadComponent)
	{
		return;
	}

	const TSubclassOf<ASquadCraftActor> DefaultCraftClass = ASquadCraftActor::StaticClass();
	const struct FCraftSpawnInfo
	{
		ESquadSlot Slot;
		TSubclassOf<ASquadCraftActor> CraftClass;
		ASquadCraftActor** TargetPointer;
	} SpawnInfos[] =
	{
		{ ESquadSlot::Left, LeftCraftClass ? LeftCraftClass : DefaultCraftClass, &LeftCraft },
		{ ESquadSlot::Center, CenterCraftClass ? CenterCraftClass : DefaultCraftClass, &CenterCraft },
		{ ESquadSlot::Right, RightCraftClass ? RightCraftClass : DefaultCraftClass, &RightCraft }
	};

	for (const FCraftSpawnInfo& SpawnInfo : SpawnInfos)
	{
		if (*SpawnInfo.TargetPointer)
		{
			continue;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ASquadCraftActor* NewCraft = GetWorld()->SpawnActor<ASquadCraftActor>(
			SpawnInfo.CraftClass,
			GetActorLocation(),
			GetActorRotation(),
			SpawnParams);

		if (!NewCraft)
		{
			continue;
		}

		NewCraft->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		NewCraft->SetAssignedSlot(SpawnInfo.Slot);
		NewCraft->SetDesiredRelativeTransform(SquadComponent->GetSlotOffset(SpawnInfo.Slot), FRotator::ZeroRotator);

		if (PlaneMesh && NewCraft->GetCraftMesh() && !NewCraft->GetCraftMesh()->GetStaticMesh())
		{
			NewCraft->GetCraftMesh()->SetStaticMesh(PlaneMesh->GetStaticMesh());
			NewCraft->GetCraftMesh()->SetRelativeScale3D(PlaneMesh->GetRelativeScale3D());
			for (int32 MaterialIndex = 0; MaterialIndex < PlaneMesh->GetNumMaterials(); ++MaterialIndex)
			{
				NewCraft->GetCraftMesh()->SetMaterial(MaterialIndex, PlaneMesh->GetMaterial(MaterialIndex));
			}
		}

		*SpawnInfo.TargetPointer = NewCraft;
	}
}

void AP_Player::RefreshSquadCrafts()
{
	if (!SquadComponent)
	{
		return;
	}

	for (ESquadSlot Slot : SquadComponent->GetAllSlots())
	{
		if (ASquadCraftActor* Craft = GetCraftForSlot(Slot))
		{
			const bool bIsActive = Slot == SquadComponent->GetActiveSlot();
			const FRotator TargetRotation = bIsActive ? Craft->GetCurrentRelativeRotation() : FRotator::ZeroRotator;

			Craft->SetActiveCraft(bIsActive);
			Craft->SetDesiredRelativeTransform(SquadComponent->GetSlotOffset(Slot), TargetRotation);
		}
	}
}

ASquadCraftActor* AP_Player::GetCraftForSlot(ESquadSlot Slot) const
{
	switch (Slot)
	{
	case ESquadSlot::Left:
		return LeftCraft;
	case ESquadSlot::Right:
		return RightCraft;
	case ESquadSlot::Center:
	default:
		return CenterCraft;
	}
}

ASquadCraftActor* AP_Player::GetActiveCraft() const
{
	return SquadComponent ? GetCraftForSlot(SquadComponent->GetActiveSlot()) : nullptr;
}

AActor* AP_Player::GetPreferredAutoFireTarget() const
{
	if (LockOn && LockOn->isLockOn && LockOn->target)
	{
		return LockOn->target;
	}

	if (UEnemyManager* EnemySubsystem = GetWorld() ? GetWorld()->GetSubsystem<UEnemyManager>() : nullptr)
	{
		const TArray<APawn*> EnemyList = EnemySubsystem->GetEnemys();
		const FVector OwnerLocation = GetActorLocation();

		AActor* BestTarget = nullptr;
		float BestDistanceSq = TNumericLimits<float>::Max();

		for (APawn* Enemy : EnemyList)
		{
			if (!Enemy || Enemy == this)
			{
				continue;
			}

			const float DistanceSq = FVector::DistSquared(OwnerLocation, Enemy->GetActorLocation());
			if (DistanceSq < BestDistanceSq)
			{
				BestDistanceSq = DistanceSq;
				BestTarget = Enemy;
			}
		}

		return BestTarget;
	}

	return nullptr;
}
