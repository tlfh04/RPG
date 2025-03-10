// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyListWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Base_Player.h"
#include "EnemyButtonWidget.h"
#include "UIManager.h"
#include "Components/SizeBox.h"

bool UEnemyListWidget::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	return true;
}

void UEnemyListWidget::ClearEnemyList()
{
	if (EnemyListBox)
	{
		EnemyListBox->ClearChildren();
	}
}

void UEnemyListWidget::AddEnemyToList(AActor* Enemy, const FString& EnemyName)
{
	if (!Enemy || !EnemyListBox || !EnemyButtonTemplate) 
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyListWidget::AddEnemyToList() - EnemyListBox 또는 EnemyButtonTemplate가 NULL입니다!"));
		return;
	}

	// 버튼 UI 생성
	UEnemyButtonWidget* EnemyButtonWidget = CreateWidget<UEnemyButtonWidget>(this, EnemyButtonTemplate);
	if (!EnemyButtonWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyListWidget::AddEnemyToList() - 버튼 위젯 생성 실패!"));
		return;
	}

	// 버튼과 텍스트 가져오기
	UButton* EnemyButton = Cast<UButton>(EnemyButtonWidget->GetWidgetFromName(TEXT("EnemyButton")));
	UTextBlock* EnemyText = Cast<UTextBlock>(EnemyButtonWidget->GetWidgetFromName(TEXT("EnemyText")));

	if (!EnemyButton || !EnemyText)
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyListWidget::AddEnemyToList() - 버튼 또는 텍스트 위젯을 찾을 수 없음!"));
		return;
	}

	// 텍스트 설정
	EnemyText->SetText(FText::FromString(EnemyName));

	// 버튼 클릭 이벤트 바인딩
	//EnemyButton->OnClicked.AddDynamic(this, &UEnemyListWidget::OnEnemyClicked);

	// UI 디버깅: 버튼 추가 전/후의 리스트 개수 확인
	int32 BeforeCount = EnemyListBox->GetChildrenCount();
	EnemyListBox->AddChildToVerticalBox(EnemyButtonWidget);
	int32 AfterCount = EnemyListBox->GetChildrenCount();

	UE_LOG(LogTemp, Warning, TEXT("UEnemyListWidget::AddEnemyToList() - 버튼 추가 완료: %s (추가 전: %d, 추가 후: %d)"), *EnemyName, BeforeCount, AfterCount);
	
	FVector DebugLocation = Enemy->GetActorLocation();
	DrawDebugBox(GetWorld(), DebugLocation, FVector(10, 10, 10), FColor::Red, false, 5.0f);

	UE_LOG(LogTemp, Warning, TEXT("Debug 위치: X=%.2f, Y=%.2f, Z=%.2f"), DebugLocation.X, DebugLocation.Y, DebugLocation.Z);
}

void UEnemyListWidget::asdf()
{
	UEnemyButtonWidget* EnemyButtonWidget = CreateWidget<UEnemyButtonWidget>(GetGameInstance()->GetWorld(), EnemyButtonTemplate);
	if (!GetGameInstance()->GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("No"));
		return;
	}

	EnemyButtonWidget->AddToViewport();
	EnemyListBox->AddChildToVerticalBox(EnemyButtonWidget);

	UE_LOG(LogTemp, Warning, TEXT("ok"));
}

void UEnemyListWidget::OnEnemyClicked()
{
	if (!SelectedEnemy) return;

	// UIManager를 통해 선택된 적을 업데이트
	UUIManager* UIManager = UUIManager::GetInstance(this);
	if (UIManager)
	{
		UIManager->SetTargetFromUI(SelectedEnemy);
		UE_LOG(LogTemp, Warning, TEXT("UEnemyListWidget: %s이(가) 선택되었습니다."), *SelectedEnemy->GetName());
	}
}
