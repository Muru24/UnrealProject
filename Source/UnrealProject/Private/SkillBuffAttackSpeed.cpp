#include "SkillBuffAttackSpeed.h"

#include "CraftAttackComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "SquadComponent.h"
#include "SquadCraftActor.h"
#include "SquadRuntimeComponent.h"
#include "TimerManager.h"

namespace
{
	void ScheduleEffectDeactivate(UNiagaraComponent* EffectComponent, UWorld* World, float EffectDuration)
	{
		if (!IsValid(EffectComponent) || !IsValid(World) || EffectDuration <= 0.0f)
		{
			return;
		}

		FTimerHandle CleanupTimerHandle;
		TWeakObjectPtr<UNiagaraComponent> WeakEffectComponent(EffectComponent);
		World->GetTimerManager().SetTimer(
			CleanupTimerHandle,
			[WeakEffectComponent]()
			{
				if (WeakEffectComponent.IsValid())
				{
					WeakEffectComponent->Deactivate();
				}
			},
			EffectDuration,
			false);
	}
}

bool USkillBuffAttackSpeed::ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration)
{
	if (!Super::ActivateSkill_Implementation(SourceActor, TargetActor, SkillDuration))
	{
		return false;
	}

	ASquadCraftActor* ActiveCraft = ResolveActiveCraft(SourceActor);
	if (!IsValid(ActiveCraft) || !ActiveCraft->IsOperational())
	{
		return false;
	}

	if (UCraftAttackComponent* AttackComponent = ActiveCraft->GetAttackComponent())
	{
		AttackComponent->ApplyTemporaryFireRateMultiplier(FireRateMultiplier, SkillDuration);
		PlayActivationEffect(ActiveCraft, SkillDuration);
		return true;
	}

	return false;
}

ASquadCraftActor* USkillBuffAttackSpeed::ResolveActiveCraft(AActor* SourceActor) const
{
	AActor* SearchActor = SourceActor;
	while (IsValid(SearchActor))
	{
		if (USquadRuntimeComponent* RuntimeComponent = SearchActor->FindComponentByClass<USquadRuntimeComponent>())
		{
			if (USquadComponent* SquadComponent = SearchActor->FindComponentByClass<USquadComponent>())
			{
				return RuntimeComponent->GetActiveCraft(SquadComponent);
			}
		}

		SearchActor = SearchActor->GetOwner();
	}

	return nullptr;
}

void USkillBuffAttackSpeed::PlayActivationEffect(AActor* TargetActor, float SkillDuration) const
{
	if (!ActivationEffect || !IsValid(TargetActor))
	{
		return;
	}

	const float EffectDuration = EffectDurationOverride > 0.0f ? EffectDurationOverride : SkillDuration;

	if (bAttachEffectToCraft)
	{
		if (UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
			ActivationEffect,
			TargetActor->GetRootComponent(),
			NAME_None,
			EffectLocationOffset,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true))
		{
			SpawnedEffect->SetWorldScale3D(EffectScale);
			ScheduleEffectDeactivate(SpawnedEffect, TargetActor->GetWorld(), EffectDuration);
		}
	}
	else
	{
		if (UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			TargetActor->GetWorld(),
			ActivationEffect,
			TargetActor->GetActorLocation() + EffectLocationOffset,
			FRotator::ZeroRotator,
			EffectScale,
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true))
		{
			SpawnedEffect->SetWorldScale3D(EffectScale);
			ScheduleEffectDeactivate(SpawnedEffect, TargetActor->GetWorld(), EffectDuration);
		}
	}
}
