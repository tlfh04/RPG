// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "EnemyButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UEnemyButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 적 정보를 설정하는 함수 (적의 이름도 함께 설정)
	void SetEnemy(AActor* NewEnemy);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* EnemyButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnemyText;  // ✅ 적의 이름을 표시할 텍스트 블록

	UPROPERTY()
	AActor* Enemy; // 버튼이 클릭되었을 때 전달할 적

	// 버튼 클릭 이벤트 처리
	UFUNCTION()
	void HandleButtonClicked();
};
