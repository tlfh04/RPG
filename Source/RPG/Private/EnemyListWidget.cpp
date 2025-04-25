// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyListWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Base_Player.h"
#include "EnemyButtonWidget.h"
#include "UIManager.h"
#include "Components/GridSlot.h"
#include "Components/SizeBox.h"


bool UEnemyListWidget::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	return true;
}

void UEnemyListWidget::ClearEnemyList()
{
	if (EnemyGridPanel)
	{
		EnemyGridPanel->ClearChildren();
	}
}

void UEnemyListWidget::AddEnemyToGrid(AActor* Enemy, const FString& EnemyName, int32 Row, int32 Column)
{
	if (!Enemy || !EnemyGridPanel || !EnemyButtonTemplate) return;

	// 버튼 UI 생성
	UEnemyButtonWidget* EnemyButtonWidget = CreateWidget<UEnemyButtonWidget>(this, EnemyButtonTemplate);
	if (!EnemyButtonWidget) return;

	// 버튼과 텍스트 가져오기
	UButton* EnemyButton = Cast<UButton>(EnemyButtonWidget->GetWidgetFromName(TEXT("EnemyButton")));
	UTextBlock* EnemyText = Cast<UTextBlock>(EnemyButtonWidget->GetWidgetFromName(TEXT("EnemyText")));

	if (!EnemyButton || !EnemyText) return;

	EnemyButtonWidget->SetEnemy(Enemy);
	
	// 리스트 박스에 버튼 위젯 추가
	UGridSlot* GridSlot = Cast<UGridSlot>(EnemyGridPanel->AddChild(EnemyButtonWidget));
	if (GridSlot)
	{
		GridSlot->SetRow(Row);
		GridSlot->SetColumn(Column);
	}
	
	AddToViewport();
}

void UEnemyListWidget::AddEnemyListToGrid(const TArray<AActor*>& Enemies)
{
	int32 NumColumns = 2;  // 2열
	int32 MaxRows = 5;     // 5행

	for (int32 i = 0; i < Enemies.Num() && i < NumColumns * MaxRows; i++)
	{
		AActor* Enemy = Enemies[i];
		int32 Row = i / NumColumns;
		int32 Col = i % NumColumns;

		AddEnemyToGrid(Enemy, Enemy->GetName(), Row, Col);
	}
}
