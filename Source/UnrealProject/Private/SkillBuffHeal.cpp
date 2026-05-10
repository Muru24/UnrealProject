#include "SkillBuffHeal.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "SquadComponent.h"
#include "SquadCraftActor.h"
#include "SquadRuntimeComponent.h"
#include "StatComponent.h"
#include "TimerManager.h"

namespace
{
	void ScheduleHealEffectDeactivate(UNiagaraComponent* EffectComponent, UWorld* World, float EffectDuration)
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

bool USkillBuffHeal::ActivateSkill_Implementation(AActor* SourceActor, AActor* TargetActor, float SkillDuration)
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

	if (UStatComponent* StatComponent = ActiveCraft->GetStatComponent())
	{
		StatComponent->Heal(HealAmount);
		PlayActivationEffect(ActiveCraft);
		return true;
	}

	return false;
}

ASquadCraftActor* USkillBuffHeal::ResolveActiveCraft(AActor* SourceActor) const
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

void USkillBuffHeal::PlayActivationEffect(AActor* TargetActor) const
{
	if (!ActivationEffect || !IsValid(TargetActor))
	{
		return;
	}

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
			ScheduleHealEffectDeactivate(SpawnedEffect, TargetActor->GetWorld(), EffectDuration);
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
			ScheduleHealEffectDeactivate(SpawnedEffect, TargetActor->GetWorld(), EffectDuration);
	}
}
}
