// Fill out your copyright notice in the Description page of Project Settings.


#include "P_Enemy.h"
#include "EnemyManager.h"
#include "UnrealProject/PathFollowerComponent.h"

AP_Enemy::AP_Enemy()
{
    PathFollower = CreateDefaultSubobject<UPathFollowerComponent>(TEXT("PathFollowerComp"));
}

void AP_Enemy::BeginPlay()
{
    GetWorld()->GetSubsystem<UEnemyManager>()->AddEnemy(this);
    APawn_Template::BeginPlay();
}
