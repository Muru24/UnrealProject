#include "BeamSpawner.h"
#include "BeamObjcet.h"
#include "Components/ChildActorComponent.h"

ABeamSpawner::ABeamSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
}

void ABeamSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ClearBeams();
	CreateBeams();
}

void ABeamSpawner::ClearBeams()
{
	for (UChildActorComponent* Comp : BeamComponents)
	{
		if (IsValid(Comp))
		{
			Comp->DestroyComponent();
		}
	}
	BeamComponents.Empty();

	// 잔여 ChildActorComponent 강제 정리
	TArray<USceneComponent*> ChildComps;
	SceneRoot->GetChildrenComponents(false, ChildComps);
	for (USceneComponent* Child : ChildComps)
	{
		if (UChildActorComponent* ChildActorComp = Cast<UChildActorComponent>(Child))
		{
			ChildActorComp->DestroyComponent();
		}
	}
}

void ABeamSpawner::CreateBeams()
{
	if (!BeamClass) return;

	// 배치 축 벡터 결정 (Y축 또는 Z축으로 일열 배치)
	const FVector AxisDir = bAlignOnZAxis ? FVector(0.0f, 0.0f, 1.0f) : FVector(0.0f, 1.0f, 0.0f);

	// 첫 번째 빔의 시작 오프셋 계산
	float StartOffset = 0.0f;
	if (bCenterAlignment && BeamCount > 1)
	{
		StartOffset = -BeamSpacing * (BeamCount - 1) * 0.5f;
	}

	for (int32 i = 0; i < BeamCount; ++i)
	{
		FName CompName = *FString::Printf(TEXT("BeamComp_%d"), i);
		UChildActorComponent* NewBeamComp = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass(), CompName, RF_Transactional);

		if (NewBeamComp)
		{
			NewBeamComp->SetChildActorClass(BeamClass);
			NewBeamComp->SetupAttachment(SceneRoot);

			// Y(또는 Z)축 방향으로 일렬 위치 오프셋 적용
			const float Offset = StartOffset + BeamSpacing * i;
			NewBeamComp->SetRelativeLocation(AxisDir * Offset);

			NewBeamComp->RegisterComponent();
			BeamComponents.Add(NewBeamComp);
		}
	}
}
