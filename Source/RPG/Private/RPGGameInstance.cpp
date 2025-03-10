// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGGameInstance.h"

#include "UIManager.h"

void URPGGameInstance::Init()
{
	Super::Init();

	// UI Manager 인스턴스 가져옴
	UUIManager* UIManager = UUIManager::GetInstance(this);
	if (UIManager)
	{
            // 위젯 블루프린트를 UI Manager에게 넘겨줌
		UIManager->SetEnemyListWidgetClass(EnemyListWidgetClass);

        	// UI 초기화 후 화면에 표시
		UIManager->InitializeUI(GetWorld());
        	
		UE_LOG(LogTemp, Warning, TEXT("UUIManager가 정상적으로 생성되었습니다."));
	}
}
