// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestTriggerBox.h"

#include "Base_Player.h"
#include "EnemySpawner.h"
#include "UIManager.h"
#include "Kismet/GameplayStatics.h"

AQuestTriggerBox::AQuestTriggerBox()
{
	OnActorBeginOverlap.AddDynamic(this, &AQuestTriggerBox::OnBeginOverlap);
}

void AQuestTriggerBox::BeginPlay()
{
	Super::BeginPlay();
}

void AQuestTriggerBox::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ABase_Player* Player = Cast<ABase_Player>(OtherActor);

	UWorld* World = GetWorld();
	
	if (!World) return;
	if (!OtherActor) return;
	if (!Player) return;
	

	if (UUIManager* UIManager = UUIManager::GetInstance(this))
	{
		UIManager->ChangeQuestState();
		Player->OverlapTriggerBox();
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(World, AEnemySpawner::StaticClass(), Found);

		if (Found.Num() > 0)
		{
			if (AEnemySpawner* Spawner = Cast<AEnemySpawner>(Found[0]))
			{
				Spawner->SpawnEnemies();
			}
		}
		Destroy();
	}
	
}
