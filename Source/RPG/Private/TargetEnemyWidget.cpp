// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetEnemyWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UTargetEnemyWidget::UpdateTargetInfo(float CurrentHP, float MaxHP)
{

	if (EnemyHPBar)
	{
		float Percent = MaxHP > 0 ? FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f) : 0.f;
		EnemyHPBar->SetPercent(Percent);
	}
}

void UTargetEnemyWidget::SetEnemyName(const FString& EnemyName)
{
	if (EnemyNameText)
	{
		EnemyNameText->SetText(FText::FromString(EnemyName));
	}
}
