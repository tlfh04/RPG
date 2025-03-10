// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManager.h"
#include "Blueprint/UserWidget.h"
#include "EnemyListWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Base_Player.h"
#include "EnemyStateComponent.h"

UUIManager* UUIManager::Instance = nullptr; // 초기화

void UUIManager::InitializeUI(UWorld* World)
{
	if (!EnemyListWidgetInstance)
	{
		EnemyListWidgetInstance = CreateWidget<UUserWidget>(World, EnemyListWidgetClass);
		if (EnemyListWidgetInstance)
		{
			EnemyListWidgetInstance->AddToViewport();
			EnemyListWidgetInstance->SetVisibility(ESlateVisibility::Visible);
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

void UUIManager::ShowEnemyListUI()
{
	if (EnemyListWidgetInstance)
	{
		EnemyListWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void UUIManager::HideEnemyListUI()
{
	if (EnemyListWidgetInstance)
	{
		EnemyListWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUIManager::UpdateEnemyList(const TArray<AActor*>& EnemyList)
{
	if (!EnemyListWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UUIManager::UpdateEnemyList() - EnemyListWidgetInstance가 NULL입니다."));
		return;
	}

	// UI 위젯 캐스팅
	UEnemyListWidget* EnemyListWidget = Cast<UEnemyListWidget>(EnemyListWidgetInstance);
	if (!EnemyListWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("UUIManager::UpdateEnemyList() - Cast<UEnemyListWidget>() 실패!"));
		return;
	}

	// 디버깅: 전달된 적 개수 확인
	UE_LOG(LogTemp, Warning, TEXT("UUIManager::UpdateEnemyList() 실행 - 받은 적 개수: %d"), EnemyList.Num());

	// 기존 버튼 삭제
	EnemyListWidget->ClearEnemyList();

	for (AActor* Enemy : EnemyList)
	{
		if (!Enemy) continue;

		UEnemyStateComponent* EnemyState = Enemy->FindComponentByClass<UEnemyStateComponent>();
		FString EnemyName = EnemyState ? EnemyState->GetEnemyName() : TEXT("Unknown Enemy");

		// 적 정보 추가
		EnemyListWidget->AddEnemyToList(Enemy, EnemyName);
		EnemyListWidget->asdf();
	}

	UE_LOG(LogTemp, Warning, TEXT("UUIManager::UpdateEnemyList() - 적 목록 UI 업데이트 완료."));
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
