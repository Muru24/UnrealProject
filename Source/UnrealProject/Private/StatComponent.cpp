// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"


UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UStatComponent::ApplyDamage(float DamageAmount)
{
    Stats.unit.HP = FMath::Clamp(Stats.unit.HP 
        - DamageAmount, 0.0f, Stats.unit.MaxHP);

    // UI¿¡ ¾Ë¸²
    OnHpChanged.Broadcast(Stats.unit.HP);

    if (Stats.unit.HP <= 0.0f)
    {
        GetOwner()->Destroy();
    }
}


