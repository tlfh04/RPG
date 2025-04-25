// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerHPWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/GridPanel.h"
#include "EnemyListWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UEnemyListWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 적 목록 초기화
	void ClearEnemyList();

	// 적 목록에 새로운 적 추가
	UFUNCTION()
	void AddEnemyToGrid(AActor* Enemy, const FString& EnemyName, int32 Row, int32 Column);
	UFUNCTION()
	void AddEnemyListToGrid(const TArray<AActor*>& Enemies);

protected:
	virtual bool Initialize() override;

private:

	UPROPERTY(meta = (BindWidget))
	UGridPanel* EnemyGridPanel;

	// 버튼 스타일을 저장하는 템플릿
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> EnemyButtonTemplate;

private:
	// 현재 선택된 적
	UPROPERTY()
	AActor* SelectedEnemy; // 클릭된 적 정보 저장
};
