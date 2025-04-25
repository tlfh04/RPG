// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHPWidget.h"
#include "Components/ProgressBar.h"

void UPlayerHPWidget::UpdateHPBar(float CurrentHP, float MaxHP)
{
	if (HPBar)
	{
		float Percent = FMath::Clamp(CurrentHP/MaxHP,0.f,1.f);
		HPBar->SetPercent(Percent);
	}
}

