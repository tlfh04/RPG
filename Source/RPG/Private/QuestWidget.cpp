// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestWidget.h"

#include "Base_Player.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

bool UQuestWidget::Initialize()
{
	if (!Super::Initialize())
		return false;

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UQuestWidget::OnCloseClicked);
	}

	// 퀘스트 받기전엔 진행도 숨김
	if (ProgressText)
	{
		ProgressText->SetVisibility(ESlateVisibility::Hidden);
	}
	if (QuestContentText)
	{
		QuestContentText->SetVisibility(ESlateVisibility::Hidden);
	}
	return true;
}

void UQuestWidget::OnCloseClicked()
{
	// 클리어 했다면 리턴
	if (ClearQuest) return;
	
	APlayerController* PC = GetOwningPlayer();
	ABase_Player* Player = Cast<ABase_Player>(PC->GetPawn());
	if (!AcceptQuest)
	{
		// 퀘스트가 받아지지 않았다면 npc한테 이동
		if (Player)
		{
			Player->MoveToNpc();
			AcceptQuest = true;
			return;
		}
	}
	if (AcceptQuest && !StartQuest)
	{
		if (Player)
		{
			// 트리거 박스 위치로 이동
			Player->MoveToTriggerBox();
		}
	}
	if (StartQuest && !EndQuest)
	{
		if (Player)
		{
			// 자동 공격 시작
			Player->StartAutoAttack();
		}
	}
	if (EndQuest)
	{
		ClearQuest = true;
		QuestContentText->SetVisibility(ESlateVisibility::Hidden);
		ProgressText->SetText(FText::FromString(TEXT("완료")));
	}
}

void UQuestWidget::ShowQuestName(const FString& InQuestName)
{
	QuestNameText->SetText(FText::FromString(InQuestName));
	
}

void UQuestWidget::UpdateQuestInfo(const FString& ContentText , int32 CurrentCount, int32 TotalCount)
{
	if (ProgressText)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (ABase_Player* Player = Cast<ABase_Player>(PC->GetPawn()))
			{
				Player->bIsQuest = true;
			}
		}
		if (StartQuest)
		{
			ProgressText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCount, TotalCount)));
			ProgressText->SetVisibility(ESlateVisibility::Visible);
		}
		QuestContentText->SetText(FText::FromString(ContentText));
		QuestContentText->SetVisibility(ESlateVisibility::Visible);
		if (CurrentCount == TotalCount)
		{
			EndQuest = true;
		}
	}
}
