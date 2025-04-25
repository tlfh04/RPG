// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManager.h"
#include "QuestWidget.h"
#include "Blueprint/UserWidget.h"
#include "EnemyListWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Base_Player.h"
#include "EnemyStateComponent.h"
#include "PlayerHPWidget.h"

void UUIManager::InitializeUI(UWorld* World)
{
	
	if (!PlayerHPWidgetInstance)
	{
		PlayerHPWidgetInstance = CreateWidget<UUserWidget>(World, PlayerHPWidgetClass);
		if (PlayerHPWidgetInstance)
		{
			PlayerHPWidgetInstance->AddToViewport();
			PlayerHPWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (!EnemyListWidgetInstance)
	{
		EnemyListWidgetInstance = CreateWidget<UUserWidget>(World, EnemyListWidgetClass);
		if (EnemyListWidgetInstance)
		{
			EnemyListWidgetInstance->AddToViewport();
			EnemyListWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	if (!QuestWidgetInstance)
	{
		QuestWidgetInstance = CreateWidget<UUserWidget>(World, QuestWidgetClass);
		if (QuestWidgetInstance)
		{
			QuestWidgetInstance->AddToViewport();
			ShowQuestName();
			QuestWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

UUIManager* UUIManager::GetInstance(UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance ? GameInstance->GetSubsystem<UUIManager>() : nullptr;
}

void UUIManager::ShowQuestName()
{
	UQuestWidget* QuestWidget = Cast<UQuestWidget>(QuestWidgetInstance);
	
	if (!QuestWidget) return;
	
	QuestWidget->ShowQuestName(QuestName);
}

void UUIManager::ShowQuestProgress(int32 CurrentCount, int32 TotalCount)
{
	UQuestWidget* QuestWidget = Cast<UQuestWidget>(QuestWidgetInstance);
	
	if (!QuestWidget) return;

	if (QuestWidget->AcceptQuest)
	{
		QuestWidget->UpdateQuestInfo(QuestContent1, CurrentCount, TotalCount);
	}
	if (QuestWidget->StartQuest)
	{
		QuestWidget->UpdateQuestInfo(QuestContent2, CurrentCount, TotalCount);
	}
}

void UUIManager::ChangeQuestState()
{
	UQuestWidget* QuestWidget = Cast<UQuestWidget>(QuestWidgetInstance);
	QuestWidget->StartQuest = true;
}


void UUIManager::ShowEnemyListUI()
{
	if (EnemyListWidgetInstance)
	{
		EnemyListWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}

	if (PlayerHPWidgetInstance)
	{
		PlayerHPWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void UUIManager::HideEnemyListUI()
{
	if (EnemyListWidgetInstance)
	{
		EnemyListWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (PlayerHPWidgetInstance)
	{
		PlayerHPWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUIManager::UpdateEnemyList(const TArray<AActor*>& EnemyList)
{
	if (!EnemyListWidgetInstance) return;

	// UI 위젯 캐스팅
	UEnemyListWidget* EnemyListWidget = Cast<UEnemyListWidget>(EnemyListWidgetInstance);

	if (!EnemyListWidget) return;
	
	// 기존 버튼 삭제
	EnemyListWidget->ClearEnemyList();
	
	EnemyListWidget->AddEnemyListToGrid(EnemyList);
	
}

void UUIManager::UpdatePlayerHP(float CurrentHP,float MaxHP)
{
	if (!PlayerHPWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("플레이어 HP 위젯 객체가 없습니다!"));
		return;
	}
	
	if (UPlayerHPWidget* PlayerHPWidget = Cast<UPlayerHPWidget>(PlayerHPWidgetInstance))
	{
		PlayerHPWidget->UpdateHPBar(CurrentHP, MaxHP);
	}
}

void UUIManager::SetTargetFromUI(AActor* SelectedEnemy)
{
	if (!SelectedEnemy) return;

	ABase_Player* Player = Cast<ABase_Player>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Player)
	{
		Player->SetTargetEnemy(SelectedEnemy);
		UE_LOG(LogTemp, Warning, TEXT("UUIManager: %s을(를) 타겟으로 설정"), *SelectedEnemy->GetName());
	}
}

void UUIManager::SetEnemyListWidgetClass(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!WidgetClass) return;

	// UI 클래스를 저장
	EnemyListWidgetClass = WidgetClass;
}

void UUIManager::SetPlayerHPWidgetClass(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!WidgetClass) return;

	// UI 클래스를 저장
	PlayerHPWidgetClass = WidgetClass;
}

void UUIManager::SetQuestWidgetClass(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!WidgetClass) return;

	// UI 클래스를 저장
	QuestWidgetClass = WidgetClass;
}
