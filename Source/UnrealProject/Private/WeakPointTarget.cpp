#include "WeakPointTarget.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "StatComponent.h"

AWeakPointTarget::AWeakPointTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. 히트박스 (루트 컴포넌트) 생성
	HitSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HitSphere"));
	SetRootComponent(HitSphere);
	
	// 구체 크기 기본값 (블루프린트에서 조절 가능)
	HitSphere->SetSphereRadius(50.0f);
	
	// 물리적인 길막은 하지 않고(QueryOnly), 오버랩(Overlap)만 감지하도록 설정
	HitSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitSphere->SetCollisionResponseToAllChannels(ECR_Overlap);

	// 2. UI 위젯 컴포넌트 생성
	TargetWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetWidget"));
	TargetWidget->SetupAttachment(HitSphere);
	
	// 위젯이 화면 공간(Screen Space)에 그려지도록 설정 (화면에 딱 달라붙어 보임)
	TargetWidget->SetWidgetSpace(EWidgetSpace::Screen);
	TargetWidget->SetDrawAtDesiredSize(true);

	// 3. 스탯 컴포넌트 생성 (총알과 데미지 연동)
	StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComp"));
}

void AWeakPointTarget::BeginPlay()
{
	Super::BeginPlay();

	// 체력 초기화
	if (StatComp)
	{
		StatComp->Stats.unit.HP = InitialHP;
		StatComp->Stats.unit.MaxHP = InitialHP;

		// 체력 변경 이벤트 바인딩
		StatComp->OnHpChanged.AddDynamic(this, &AWeakPointTarget::OnHealthChanged);
	}
}

void AWeakPointTarget::OnHealthChanged(float CurrentHp)
{
	if (bIsDestroyed) return;

	if (CurrentHp <= 0.0f)
	{
		bIsDestroyed = true;

		// 1. 더 이상 총알에 맞지 않도록 콜리전 비활성화
		if (HitSphere)
		{
			HitSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// 2. 과녁 UI 숨기기
		if (TargetWidget)
		{
			TargetWidget->SetVisibility(false);
		}

		// 3. 파괴 이벤트 브로드캐스트 (보스가 듣고 무력화 상태로 전환)
		OnTargetDestroyed.Broadcast(this);
	}
}

float AWeakPointTarget::GetHealthPercent() const
{
	if (!StatComp || StatComp->Stats.unit.MaxHP <= 0.0f) return 0.0f;
	
	return StatComp->Stats.unit.HP / StatComp->Stats.unit.MaxHP;
}
