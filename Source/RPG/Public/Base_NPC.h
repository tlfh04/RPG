// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Base_NPC.generated.h"

UCLASS()
class RPG_API ABase_NPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABase_NPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
