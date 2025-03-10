// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyStateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UEnemyStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// 몬스터 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Monster Stats")
	FString EnemyName;

	// 체력 (HP)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Monster Stats")
	float MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Monster Stats")
	float CurrentHP;

	// 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Monster Stats")
	float AttackPower;

	// 체력 감소 함수
	UFUNCTION(BlueprintCallable, Category="Damage")
	void TakeDamage(float Damage);
	
	// 적 이름 호출
	FString GetEnemyName() const;
		
};
