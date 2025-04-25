// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "EnemyStateComponent.h"
#include "Base_Enemy.generated.h"

UCLASS()
class RPG_API ABase_Enemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABase_Enemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	///////////////// Init Var //////////////////

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* OverlayMaterial;  // Overlay Material

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* DefaultMaterial;  // 기본 Material

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* BasicAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere, Category= "UI")
	class UWidgetComponent* StateUIWidgetComponent;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool bIsDead;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsAttacking = false;

	//////////////// Function //////////////////
	
	void SetOverlayMaterial(bool bEnable);
	
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,class AActor* DamageCauser) override;

	void StateVisible();

	void StateHidden();
	
	void Die();

	void StateUpdate();

	/////////////// GETTER ////////////////////

	bool IsDead() const { return bIsDead; }
};
