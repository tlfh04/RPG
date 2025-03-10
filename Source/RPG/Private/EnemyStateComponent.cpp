// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStateComponent.h"

// Sets default values for this component's properties
UEnemyStateComponent::UEnemyStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	// Init State;
	CurrentHP = MaxHP;
	EnemyName = TEXT("Unknown Enemy");
}


// Called when the game starts
void UEnemyStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEnemyStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEnemyStateComponent::TakeDamage(float Damage)
{
	CurrentHP -= Damage;
	if (CurrentHP <= 0.0f)
	{
		CurrentHP = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("%s Die"), *EnemyName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s HP: %f"), *EnemyName, CurrentHP);
	}
}

FString UEnemyStateComponent::GetEnemyName() const
{
	return EnemyName;
}