#include "ChaosDestructiblePart.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GeometryCollection/GeometryCollectionSimulationTypes.h"
#include "Components/BoxComponent.h"
#include "StatComponent.h"

AChaosDestructiblePart::AChaosDestructiblePart()
{
	PrimaryActorTick.bCanEverTick = false;

	GeomCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeomCollection"));
	SetRootComponent(GeomCollection);
	GeomCollection->SetNotifyRigidBodyCollision(true);

	GeomCollection->SetSimulatePhysics(true);

	// 카오스 메쉬 본체의 오버랩은 끄고(최적화), 자식인 HitCollider(Box)에서만 충돌을 감지하도록 설정
	GeomCollection->SetGenerateOverlapEvents(false);

	GeomCollection->InitialVelocityType = EInitialVelocityTypeEnum::Chaos_Initial_Velocity_None;
	
	GeomCollection->ObjectType = EObjectStateTypeEnum::Chaos_Object_Static;

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("HitCollider"));
	HitCollider->SetupAttachment(GeomCollection);
	
	HitCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	HitCollider->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	HitCollider->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);

	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComp"));
}

void AChaosDestructiblePart::BeginPlay()
{
	Super::BeginPlay();

	// 카오스 컴포넌트는 엔진 초기화 시점에 물리가 켜져있어야 충돌(Strain) 연산이 즉시 가능합니다.
	// BP에 저장된 캐시를 무시하고 런타임에 강제로 활성화합니다.
	if (GeomCollection)
	{
		GeomCollection->SetSimulatePhysics(true);
	}

	// InitialHP를 StatComp에 적용 (총알 데미지 1.0 기준 → InitialHP발에 파괴)
	StatComp->Stats.unit.HP = InitialHP;
	StatComp->Stats.unit.MaxHP = InitialHP;

	// StatComp HP 변화 이벤트: 데미지를 받아 0이 될 때 전체 파괴 효과 연동
	StatComp->OnHpChanged.AddDynamic(this, &AChaosDestructiblePart::OnHealthChanged);
}

void AChaosDestructiblePart::OnHealthChanged(float CurrentHp)
{
	if (bIsDestroyed) return;

	if (CurrentHp <= 0.0f)
	{
		bIsDestroyed = true;

		if (HitCollider)
		{
			HitCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		Unanchor();

		OnPartFullyDestroyed.Broadcast(this);
	}
}

void AChaosDestructiblePart::Unanchor()
{
	if (!GeomCollection) return;

	// 중력을 끄고 물리 시뮬레이션 활성화 (제자리에서 무중력으로 터지게 함)
	GeomCollection->SetEnableGravity(false);
	GeomCollection->SetSimulatePhysics(true);

	// ★ 필살기: 언리얼 내부의 어떠한 데미지 방어 설정이나 고정(Anchor) 설정도 무시하고
	// 반경 내의 모든 카오스 조각들을 강제로 동적(Dynamic) 상태로 깨워버립니다.
	GeomCollection->ApplyKinematicField(999999.0f, GetActorLocation());

	// 깨어난 조각들에게 흩어지도록 엄청난 충격량 적용
	GeomCollection->ApplyExternalStrain(
		INDEX_NONE,
		GetActorLocation(),
		99999.0f,   // 아주 넓은 범위
		10,         // 깊게 전파
		1.0f,       // 충격량
		10000000000000.0f  // 엄청난 힘
	);
}
