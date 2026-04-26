#include "HomingMovementComponent.h"

#include "BulletBase.h"

UHomingMovementComponent::UHomingMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHomingMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ABulletBase* BulletOwner = Cast<ABulletBase>(GetOwner());
	if (!IsValid(BulletOwner))
	{
		return;
	}

	AActor* CurrentTarget = BulletOwner->GetTarget();
	if (!IsValid(CurrentTarget))
	{
		BulletOwner->SetTarget(nullptr);
		CurrentTarget = nullptr;
	}

	if (CurrentTarget)
	{
		const FVector Direction = (CurrentTarget->GetActorLocation() - BulletOwner->GetActorLocation()).GetSafeNormal();
		const FRotator TargetRotation = Direction.Rotation();
		const FRotator NewRotation = FMath::RInterpTo(BulletOwner->GetActorRotation(), TargetRotation, DeltaTime, 20.0f);
		BulletOwner->SetActorRotation(NewRotation);
	}

	const FVector MoveStep = BulletOwner->GetActorForwardVector() * BulletOwner->GetSpeed() * DeltaTime;
	FHitResult SweepHit;
	BulletOwner->AddActorWorldOffset(MoveStep, true, &SweepHit);
}
