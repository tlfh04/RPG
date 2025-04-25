// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyButtonWidget.h"

#include "EnemyStateComponent.h"
#include "UIManager.h"


void UEnemyButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (EnemyButton)
	{
		// 버튼에 바인딩
		EnemyButton->OnClicked.AddDynamic(this, &UEnemyButtonWidget::HandleButtonClicked);
	}
}

void UEnemyButtonWidget::SetEnemy(AActor* NewEnemy)
{
	Enemy = NewEnemy;
	
	UEnemyStateComponent* EnemyState = Enemy->FindComponentByClass<UEnemyStateComponent>();
	if (EnemyState)
	{
		FString EnemyName = EnemyState->GetEnemyName();
		EnemyText->SetText(FText::FromString(EnemyName));  // ✅ 텍스트 블록에 적의 이름 설정
	}
}

void UEnemyButtonWidget::HandleButtonClicked()
{
	if (!Enemy)
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyButtonWidget::HandleButtonClicked() - Enemy가 NULL입니다!"));
		return;
	}

	UEnemyStateComponent* EnemyState = Enemy->FindComponentByClass<UEnemyStateComponent>();

	FString EnemyName = EnemyState->GetEnemyName();
	
	UE_LOG(LogTemp, Warning, TEXT("UEnemyButtonWidget::HandleButtonClicked() - 선택한 적: %s"), *EnemyName);

	// UIManager를 통해 선택된 적을 업데이트
	UUIManager* UIManager = UUIManager::GetInstance(this);
	if (UIManager)
	{
		UIManager->SetTargetFromUI(Enemy);
		UE_LOG(LogTemp, Warning, TEXT("타겟이 설정되었습니다: %s"), *EnemyName);
	}
}
