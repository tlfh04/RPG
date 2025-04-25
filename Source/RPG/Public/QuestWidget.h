// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	// 퀘스트 이름
	UFUNCTION(Category="QuestUI")
	void ShowQuestName(const FString& InQuestName);

	// 퀘스트 업데이트 함수
	UFUNCTION(Category="QuestUI")
	void UpdateQuestInfo(const FString& ContentText , int32 CurrentCount, int32 TotalCount);

protected:
	virtual bool Initialize() override;

	// 위젯 바인드
	UPROPERTY(meta=(BindWidget)) class UTextBlock* QuestNameText;
	UPROPERTY(meta=(BindWidget)) class UTextBlock* QuestContentText;
	UPROPERTY(meta=(BindWidget)) class UTextBlock* ProgressText;
	UPROPERTY(meta=(BindWidget)) class UButton* CloseButton;

public:
	bool AcceptQuest = false;
	bool StartQuest = false;
	bool EndQuest = false;
	bool ClearQuest = false;

private:
	UFUNCTION()
	void OnCloseClicked();
	
};
