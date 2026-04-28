// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_CompositeMaster.h"
#include "EnemyManager.h"
#include "UnrealProject/Pawn_Template.h"

APawn_CompositeMaster::APawn_CompositeMaster()
{
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

    RootComponent = SceneRoot;
}

void APawn_CompositeMaster::Tick(float DeltaTime)
{
}

void APawn_CompositeMaster::BeginPlay()
{
    Super::BeginPlay();
    
    ChildParts.Empty();
    GetComponents<UChildActorComponent>(ChildParts);

	UEnemyManager* EnemyManager = GetWorld()->GetSubsystem<UEnemyManager>();

	if (!ChildParts.IsEmpty())
	{
        for (UChildActorComponent* enemy : ChildParts)
        {
            if (IsValid(enemy))
            {
                APawn* PEnemy = Cast<APawn_Template>(enemy->GetChildActor());

                if (PEnemy)
                {
                    EnemyManager->AddEnemy(PEnemy);
                }
            }
        }
	}
}
