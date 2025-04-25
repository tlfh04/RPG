// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_NPC.h"

// Sets default values
ABase_NPC::ABase_NPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABase_NPC::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("NPC"));
}
