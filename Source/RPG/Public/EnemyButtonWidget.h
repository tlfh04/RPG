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
	UPROPERTY(meta = (BindWidget))
	UButton* EnemyButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnemyText;
};
