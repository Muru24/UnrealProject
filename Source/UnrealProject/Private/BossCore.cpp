#include "BossCore.h"
#include "Snake_CompositeMaster.h"

ABossCore::ABossCore()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABossCore::BeginPlay()
{
	Super::BeginPlay();

    // 1. 스네이크 보스 소환
    if (SnakeClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();

        // 코어와 동일한 위치/회전으로 소환
        SpawnedSnake = GetWorld()->SpawnActor<ASnake_CompositeMaster>(SnakeClass, GetActorLocation(), GetActorRotation(), SpawnParams);

        if (SpawnedSnake)
        {
            // 2. 스네이크의 궤도 중심점(CenterActor)을 이 코어로 설정
            SpawnedSnake->CenterActor = this;
            
            UE_LOG(LogTemp, Warning, TEXT("Snake Boss Spawned and Orbit Center set to BossCore!"));
        }
    }
}
