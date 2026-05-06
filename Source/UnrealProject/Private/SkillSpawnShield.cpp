#include "SkillSpawnShield.h"
#include "Shild.h"
#include "SquadRuntimeComponent.h"
#include "SquadComponent.h"
#include "SquadCraftActor.h"
#include "Engine/World.h"

bool USkillSpawnShield::ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration)
{
	if (!Super::ActivateSkill_Implementation(SourceActor, TargetActor, SkillDuration) || !IsValid(SourceActor))
	{
		return false;
	}

	UWorld* World = SourceActor->GetWorld();
	if (!World)
	{
		return false;
	}

	// 1. 방패를 부착할 활성 기체 찾기
	AActor* AttachmentTarget = ResolveActiveActor(SourceActor);
	if (!IsValid(AttachmentTarget))
	{
		// 활성 기체를 못 찾으면 소환자(SourceActor)를 기준으로 함
		AttachmentTarget = SourceActor;
	}

	// 2. 방패 소환 위치 및 회전 설정
	// 센터 기체의 정면 방향으로 오프셋 적용
	const FVector SpawnLocation = AttachmentTarget->GetActorLocation() + AttachmentTarget->GetActorForwardVector() * ForwardOffset;
	const FRotator SpawnRotation = AttachmentTarget->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = SourceActor;
	SpawnParams.Instigator = Cast<APawn>(SourceActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 3. 방패 액터 생성
	TSubclassOf<AShild> ActualShieldClass = ShieldClass;
	if (!ActualShieldClass)
	{
		ActualShieldClass = AShild::StaticClass();
	}

	AShild* SpawnedShield = World->SpawnActor<AShild>(ActualShieldClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (IsValid(SpawnedShield))
	{
		// 4. 수명(LifeTime) 설정
		SpawnedShield->LifeTime = SkillDuration;

		// 5. 센터 기체에 자식으로 부착 (기체가 이동하면 방패도 같이 이동)
		SpawnedShield->AttachToActor(AttachmentTarget, FAttachmentTransformRules::KeepWorldTransform);

		UE_LOG(LogTemp, Warning, TEXT("Shield Skill Activated! Attached to: %s"), *AttachmentTarget->GetName());
		return true;
	}

	return false;
}

AActor* USkillSpawnShield::ResolveActiveActor(AActor* SourceActor) const
{
	if (!IsValid(SourceActor)) return nullptr;

	// 소환자 또는 소환자의 Owner(PlayerPawn 등)로부터 컴포넌트 탐색
	AActor* SearchActor = SourceActor;
	while (SearchActor)
	{
		USquadRuntimeComponent* RuntimeComp = SearchActor->FindComponentByClass<USquadRuntimeComponent>();
		USquadComponent* SquadComp = SearchActor->FindComponentByClass<USquadComponent>();

		if (RuntimeComp && SquadComp)
		{
			// 현재 플레이어가 조종 중인(Active) 기체 반환
			return RuntimeComp->GetActiveCraft(SquadComp);
		}
		SearchActor = SearchActor->GetOwner();
	}

	return nullptr;
}
