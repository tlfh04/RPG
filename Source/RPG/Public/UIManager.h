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
	
	// 서브시스템 초기화 함수
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// UI 초기화
	void InitializeUI(UWorld* World);

	// UIManager 인스턴스를 가져오는 함수 (싱글톤처럼 사용 가능)
	static UUIManager* GetInstance(UObject* WorldContextObject);
	
	void ShowEnemyListUI(); // UI 표시
	void HideEnemyListUI(); // UI 숨김
	void UpdateEnemyList(const TArray<AActor*>& EnemyList); // 적 목록 업데이트
	void SetTargetFromUI(AActor* SelectedEnemy); // UI에서 클릭한 적을 타겟으로 설정

	void SetEnemyListWidgetClass(TSubclassOf<UUserWidget> WidgetClass); // 블루프린트에서 UI 설정


protected:
	
	UPROPERTY()
	UUserWidget* EnemyListWidgetInstance;
	UPROPERTY()
	TSubclassOf<UUserWidget> EnemyListWidgetClass;
private:
	static UUIManager* Instance; // 싱글턴 인스턴스
};
