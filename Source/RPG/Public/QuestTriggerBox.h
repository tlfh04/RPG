// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "QuestTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API AQuestTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	AQuestTriggerBox();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
};
