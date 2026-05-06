#include "BossSupportPartOrbitComponent.h"

#include "BossCore.h"
#include "BossOutPart.h"
#include "Components/ChildActorComponent.h"
#include "Pawn_CompositeMaster.h"

UBossSupportPartOrbitComponent::UBossSupportPartOrbitComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBossSupportPartOrbitComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCompositeMaster = Cast<APawn_CompositeMaster>(GetOwner());
	if (bAutoRefreshOnBeginPlay)
	{
		RefreshOrbitParts();
	}
}

void UBossSupportPartOrbitComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bOrbitEnabled)
	{
		return;
	}

	if (OrbitPartStates.Num() == 0)
	{
		RefreshOrbitParts();
	}

	UpdateOrbit(DeltaTime);
}

void UBossSupportPartOrbitComponent::RefreshOrbitParts()
{
	OrbitPartStates.Reset();
	CurrentOrbitAngleRadians = 0.0f;
	SpeedBurstIntervalTimer = 0.0f;
	SpeedBurstDurationTimer = 0.0f;
	bSpeedBurstActive = false;

	if (!OwnerCompositeMaster)
	{
		OwnerCompositeMaster = Cast<APawn_CompositeMaster>(GetOwner());
	}

	if (!OwnerCompositeMaster)
	{
		return;
	}

	ABossCore* BossCore = OwnerCompositeMaster->GetBossCore();
	if (!IsValid(BossCore))
	{
		return;
	}

	TArray<UChildActorComponent*> ChildParts;
	OwnerCompositeMaster->GetChildPartComponents(ChildParts);

	const FVector CenterLocation = BossCore->GetActorLocation();
	const FVector OrbitForward = BossCore->GetActorForwardVector();
	const FVector OrbitRight = BossCore->GetActorRightVector();
	const FVector OrbitUp = BossCore->GetActorUpVector();

	for (UChildActorComponent* ChildPart : ChildParts)
	{
		if (!IsValid(ChildPart) || !IsValid(ChildPart->GetChildActor()))
		{
			continue;
		}

		if (!Cast<ABossOutPart>(ChildPart->GetChildActor()))
		{
			continue;
		}

		const FVector ToPart = ChildPart->GetComponentLocation() - CenterLocation;
		const float HeightOffset = FVector::DotProduct(ToPart, OrbitUp);
		const FVector HorizontalOffset = ToPart - (OrbitUp * HeightOffset);
		const float OrbitRadius = HorizontalOffset.Size();

		if (OrbitRadius <= KINDA_SMALL_NUMBER)
		{
			continue;
		}

		const FVector HorizontalDirection = HorizontalOffset / OrbitRadius;
		const float AngleOffsetRadians = FMath::Atan2(
			FVector::DotProduct(HorizontalDirection, OrbitRight),
			FVector::DotProduct(HorizontalDirection, OrbitForward));

		FBossSupportPartOrbitState& NewState = OrbitPartStates.AddDefaulted_GetRef();
		NewState.PartComponent = ChildPart;
		NewState.AngleOffsetRadians = AngleOffsetRadians;
		NewState.OrbitRadius = OrbitRadius;
		NewState.HeightOffset = HeightOffset;
	}
}

void UBossSupportPartOrbitComponent::UpdateOrbit(float DeltaTime)
{
	if (!OwnerCompositeMaster || OrbitPartStates.Num() == 0)
	{
		return;
	}

	ABossCore* BossCore = OwnerCompositeMaster->GetBossCore();
	if (!IsValid(BossCore))
	{
		return;
	}

	if (bSpeedBurstActive)
	{
		SpeedBurstDurationTimer += DeltaTime;
		if (SpeedBurstDurationTimer >= SpeedBurstDuration)
		{
			bSpeedBurstActive = false;
			SpeedBurstDurationTimer = 0.0f;
		}
	}
	else
	{
		SpeedBurstIntervalTimer += DeltaTime;
		if (SpeedBurstIntervalTimer >= SpeedBurstInterval)
		{
			bSpeedBurstActive = true;
			SpeedBurstIntervalTimer = 0.0f;
			SpeedBurstDurationTimer = 0.0f;
		}
	}

	const float CurrentSpeedDegrees = OrbitSpeedDegrees * (bSpeedBurstActive ? SpeedBurstMultiplier : 1.0f);
	CurrentOrbitAngleRadians += FMath::DegreesToRadians(CurrentSpeedDegrees) * DeltaTime;

	const FVector CenterLocation = BossCore->GetActorLocation();
	const FVector OrbitForward = BossCore->GetActorForwardVector();
	const FVector OrbitRight = BossCore->GetActorRightVector();
	const FVector OrbitUp = BossCore->GetActorUpVector();

	for (FBossSupportPartOrbitState& OrbitState : OrbitPartStates)
	{
		if (!IsValid(OrbitState.PartComponent))
		{
			continue;
		}

		const float OrbitAngle = CurrentOrbitAngleRadians + OrbitState.AngleOffsetRadians;
		const FVector HorizontalDirection =
			(OrbitForward * FMath::Cos(OrbitAngle)) +
			(OrbitRight * FMath::Sin(OrbitAngle));
		const FVector TargetLocation =
			CenterLocation +
			(HorizontalDirection * OrbitState.OrbitRadius) +
			(OrbitUp * OrbitState.HeightOffset);

		OrbitState.PartComponent->SetWorldLocation(TargetLocation);
	}
}
