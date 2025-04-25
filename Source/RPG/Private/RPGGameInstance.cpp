// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGGameInstance.h"

#include "UIManager.h"

void URPGGameInstance::Init()
{
	Super::Init();

	// UI Manager 인스턴스 가져옴
	UUIManager* UIManager = GetSubsystem<UUIManager>();
	if (UIManager)
	{
		// 위젯 블루프린트를 UI Manager에게 넘겨줌
		UIManager->SetEnemyListWidgetClass(EnemyListWidgetClass);
		UIManager->SetPlayerHPWidgetClass(PlayerHPWidgetClass);
		UIManager->SetQuestWidgetClass(QuestWidgetClass);
        	
		UE_LOG(LogTemp, Warning, TEXT("UUIManager가 정상적으로 생성되었습니다."));
	}
}
