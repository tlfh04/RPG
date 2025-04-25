// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h" 
#include "TargetEnemyWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UTargetEnemyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateTargetInfo(float CurrentHP, float MaxHP);

	void SetEnemyName(const FString& EnemyName);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnemyNameText; // 적 이름

	// 프로그레스바로 체력 표시
	UPROPERTY(meta = (BindWidget))
	UProgressBar* EnemyHPBar; // 적 hp바
};
