#pragma once

#include "CoreMinimal.h"
#include "Base_Enemy.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class RPG_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();
	
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SpawnEnemies(); // 적 생성
	

protected:
	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<ABase_Enemy> EnemyClass; // 스폰할 적
	
	UPROPERTY(EditAnywhere, Category = "Spawner")
	int32 SpawnCount = 10; // 스폰 갯수
	
	UPROPERTY(EditAnywhere, Category = "Spawner")
	float SpawnRadius = 1000.f; // 스폰 범위

	UPROPERTY(EditAnywhere, Category = "Spawner")
	FString SpawnedEnemyName; // 스폰 적 이름

	UPROPERTY(EditAnywhere, Category = "Spawner")
	float SpawnedEnemyMaxHP; // 스폰 적 체력

	UPROPERTY(EditAnywhere, Category = "Spawner")
	float SpawnedEnemyDamage; // 스폰 적 데미지

public:	

};
