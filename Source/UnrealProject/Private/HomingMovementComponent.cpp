#include "HomingMovementComponent.h"

#include "BulletBase.h"

UHomingMovementComponent::UHomingMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHomingMovementComponent::SetHomingActivationDelay(float InHomingActivationDelay)
{
	HomingActivationDelay = FMath::Max(0.0f, InHomingActivationDelay);
	HomingElapsedTime = 0.0f;
}

void UHomingMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	HomingElapsedTime = 0.0f;
}

void UHomingMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ABulletBase* BulletOwner = Cast<ABulletBase>(GetOwner());
	if (!IsValid(BulletOwner))
	{
		return;
	}

	HomingElapsedTime += DeltaTime;

	AActor* CurrentTarget = BulletOwner->GetTarget();
	if (!IsValid(CurrentTarget))
	{
		BulletOwner->SetTarget(nullptr);
		CurrentTarget = nullptr;
	}

	if (CurrentTarget && HomingElapsedTime >= HomingActivationDelay)
	{
		const FVector Direction = (CurrentTarget->GetActorLocation() - BulletOwner->GetActorLocation()).GetSafeNormal();
		const FRotator TargetRotation = Direction.Rotation();
		const FRotator NewRotation = FMath::RInterpTo(BulletOwner->GetActorRotation(), TargetRotation, DeltaTime, TurnInterpSpeed);
		BulletOwner->SetActorRotation(NewRotation);
	}

	const FVector MoveStep = BulletOwner->GetActorForwardVector() * BulletOwner->GetSpeed() * DeltaTime;
	FHitResult SweepHit;
	BulletOwner->AddActorWorldOffset(MoveStep, true, &SweepHit);
}
