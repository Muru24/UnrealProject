#pragma once

#include "CoreMinimal.h"
#include "UnrealProject/Pawn_Template.h"
#include "BossCore.generated.h"

class ASnake_CompositeMaster;

/**
 * 스네이크 보스의 핵심 코어 액터입니다.
 * 시작 시 스네이크를 소환하고 자신의 위치를 중심으로 궤도를 돌도록 설정합니다.
 */
UCLASS()
class UNREALPROJECT_API ABossCore : public APawn_Template
{
	GENERATED_BODY()
	
public:	
	ABossCore();

protected:
	virtual void BeginPlay() override;

public:
    /** 소환할 스네이크 보스 클래스 (BP_Snake) */
    UPROPERTY(EditAnywhere, Category = "Boss|Settings")
    TSubclassOf<ASnake_CompositeMaster> SnakeClass;

    /** 생성된 스네이크 보스 참조 */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|Runtime")
    ASnake_CompositeMaster* SpawnedSnake;
};
