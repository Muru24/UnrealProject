// Fill out your copyright notice in the Description page of Project Settings.


#include "P_Enemy.h"
#include "EnemyRushComponent.h"
#include "EnemyManager.h"

AP_Enemy::AP_Enemy()
{
    EnemyRushComponent = CreateDefaultSubobject<UEnemyRushComponent>(TEXT("EnemyRushComp"));
}

void AP_Enemy::BeginPlay()
{
    GetWorld()->GetSubsystem<UEnemyManager>()->AddEnemy(this);
    APawn_Template::BeginPlay();
}
