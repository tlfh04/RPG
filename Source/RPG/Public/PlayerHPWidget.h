// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UPlayerHPWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateHPBar(float CurrentHP, float MaxHP);

protected:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;
};
