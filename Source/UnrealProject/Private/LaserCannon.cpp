#include "LaserCannon.h"
#include "LaserAttackComponent.h"
#include "Components/SceneComponent.h"

ALaserCannon::ALaserCannon()
{
	PrimaryActorTick.bCanEverTick = false; // 컴포넌트가 틱을 수행하므로 비활성 가능

	FireOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("FireOrigin"));
	FireOrigin->SetupAttachment(RootComponent);

    LaserComponent = CreateDefaultSubobject<ULaserAttackComponent>(TEXT("LaserComponent"));
}

void ALaserCannon::BeginPlay()
{
	Super::BeginPlay();

    if (LaserComponent && FireOrigin)
    {
        LaserComponent->SetFireOrigin(FireOrigin);
    }
}

void ALaserCannon::StartBeamAttack(AActor* InTarget)
{
    if (LaserComponent)
    {
        LaserComponent->StartLaser(InTarget);
    }
}

void ALaserCannon::StopBeamAttack()
{
    if (LaserComponent)
    {
        LaserComponent->StopLaser();
    }
}

void ALaserCannon::SetBeamTarget(AActor* InTarget)
{
    if (LaserComponent)
    {
        LaserComponent->SetTargetActor(InTarget);
    }
}

AActor* ALaserCannon::GetBeamTarget() const
{
    return LaserComponent ? LaserComponent->GetTargetActor() : nullptr; 
}
