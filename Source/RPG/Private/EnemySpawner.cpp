// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "Base_Enemy.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemySpawner::SpawnEnemies()
{
	if (!EnemyClass) return;
	UWorld* World = GetWorld();
	if (!World) return;

	const float SpawnerZ = GetActorLocation().Z;

	for (int32 i = 0; i < SpawnCount; ++i)
	{
		// XY 랜덤, Z 고정
		FVector RandXY = UKismetMathLibrary::RandomUnitVector() * SpawnRadius;
		RandXY.Z = 0.f;
		FVector SpawnLoc = FVector(GetActorLocation().X + RandXY.X,GetActorLocation().Y + RandXY.Y, SpawnerZ);

		// 스폰 파라미터: 가능하다면 충돌 위치 보정
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = 
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// 적 스폰
		ABase_Enemy* NewEnemy = World->SpawnActor<ABase_Enemy>(EnemyClass, SpawnLoc,FRotator::ZeroRotator, Params);

		if (NewEnemy)
		{
			// 스테이트 컴포넌트 초기화
			if (UEnemyStateComponent* State = NewEnemy->FindComponentByClass<UEnemyStateComponent>())
			{
				State->EnemyName = SpawnedEnemyName;
				State->MaxHP = SpawnedEnemyMaxHP;
				State->CurrentHP = SpawnedEnemyMaxHP;
				State->AttackDamage = SpawnedEnemyDamage;
			}
			NewEnemy->StateUpdate();
			NewEnemy->SpawnDefaultController();
		}
	}
}


