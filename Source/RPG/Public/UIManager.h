// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIManager.generated.h"

/**
 * 
 */

UCLASS()
class RPG_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	FString QuestName = TEXT("퀘스트1");

	FString QuestContent1 = TEXT("위치로 이동하세요.");
	
	FString QuestContent2 = TEXT("기글러 처치");
	
	// 서브시스템 초기화 함수
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// UI 초기화
	void InitializeUI(UWorld* World);

	// UIManager 인스턴스를 가져오는 함수 (싱글톤처럼 사용 가능)
	static UUIManager* GetInstance(UObject* WorldContextObject);
	
	void ShowEnemyListUI(); // UI 표시
	void HideEnemyListUI(); // UI 숨김
	void UpdateEnemyList(const TArray<AActor*>& EnemyList); // 적 목록 업데이트
	void UpdatePlayerHP(float CurrentHP,float MaxHP); // 플레이어 HP 업데이트
	void SetTargetFromUI(AActor* SelectedEnemy); // UI에서 클릭한 적을 타겟으로 설정

	// 블루프린트에서 UI 설정
	void SetEnemyListWidgetClass(TSubclassOf<UUserWidget> WidgetClass); 
	void SetPlayerHPWidgetClass(TSubclassOf<UUserWidget> WidgetClass);
	void SetQuestWidgetClass(TSubclassOf<UUserWidget> WidgetClass); 
	
	UFUNCTION(Category="QuestUI")
	void ShowQuestName(); // 퀘스트 이름

	UFUNCTION(BlueprintCallable, Category="QuestUI") // 퀘스트 진행도
	void ShowQuestProgress(int32 CurrentCount, int32 TotalCount);

	UFUNCTION(Category="QuestUI")
	void ChangeQuestState(); // 퀘스트 이름

protected:
	
	UPROPERTY()
	UUserWidget* EnemyListWidgetInstance;
	UPROPERTY()
	UUserWidget* PlayerHPWidgetInstance;
	UPROPERTY()
	UUserWidget* QuestWidgetInstance;
	UPROPERTY()
	TSubclassOf<UUserWidget> EnemyListWidgetClass;
	UPROPERTY()
	TSubclassOf<UUserWidget> PlayerHPWidgetClass;
	UPROPERTY()
	TSubclassOf<UUserWidget> QuestWidgetClass;
	
};
