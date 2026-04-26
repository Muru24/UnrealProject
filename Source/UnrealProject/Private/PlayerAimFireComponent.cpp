#include "PlayerAimFireComponent.h"

#include "Engine/World.h"
#include "LockOnComponent.h"
#include "SquadCraftActor.h"

UPlayerAimFireComponent::UPlayerAimFireComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UPlayerAimFireComponent::FireActiveCraft(APlayerController* PlayerController, ULockOnComponent* LockOnComponent, ASquadCraftActor* ActiveCraft, APawn* InstigatorPawn) const
{
	if (!PlayerController || !ActiveCraft || !InstigatorPawn)
	{
		return false;
	}

	FVector TargetPoint = FVector::ZeroVector;
	AActor* TargetActor = nullptr;
	if (!ResolveAimTarget(PlayerController, LockOnComponent, TargetPoint, TargetActor))
	{
		return false;
	}

	return ActiveCraft->FireAt(TargetPoint, TargetActor, InstigatorPawn);
}

bool UPlayerAimFireComponent::ResolveAimTarget(APlayerController* PlayerController, ULockOnComponent* LockOnComponent, FVector& OutTargetPoint, AActor*& OutTargetActor) const
{
	OutTargetPoint = FVector::ZeroVector;
	OutTargetActor = nullptr;

	if (LockOnComponent && LockOnComponent->IsLockOnEnabled() && LockOnComponent->GetCurrentTarget())
	{
		OutTargetPoint = LockOnComponent->GetCurrentTarget()->GetActorLocation();
		OutTargetActor = LockOnComponent->GetCurrentTarget();
		return true;
	}

	FVector MouseLocation = FVector::ZeroVector;
	FVector MouseDirection = FVector::ZeroVector;
	if (!PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
	{
		return false;
	}

	const FVector TraceStart = MouseLocation;
	const FVector TraceEnd = TraceStart + (MouseDirection * 10000.0f);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	if (GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		OutTargetPoint = HitResult.Location;
	}
	else
	{
		OutTargetPoint = TraceEnd;
	}

	return true;
}
