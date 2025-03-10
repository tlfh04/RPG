// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Base_Player.generated.h"

// Forward Declaration
class UInputMappingContext;
class UInputAction;

UCLASS()
class RPG_API ABase_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABase_Player();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	///////////// Init Var /////////////
public:

	float TargetPitch = 0.0f; // 목표 Pitch 값
	float TargetYaw = 0.0f; // 목표 Yaw 값
	float CurrentPitchSpeed = 0.0f; // 현재 Pitch 속도
	float CurrentYawSpeed = 0.0f; // 현재 Yaw 속도
	UPROPERTY(VisibleInstanceOnly,Category="Search")
	TArray<AActor*> FoundEnemies; // "Enemy" 태그를 가진 액터들을 저장할 배열
	UPROPERTY(VisibleInstanceOnly,Category="Search")
	TArray<AActor*> ClosestEnemies; // 가까운 엑터들을 저장할 배열
	UPROPERTY()
	AActor* CurrentTarget; // 현재 타겟

	
	// Handle
	FTimerHandle CameraDampingTimerHandle;

public:

	///////////// Input /////////////

	UPROPERTY(EditAnywhere,Category="Input")
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* ToggleViewIA; // 우클릭 인풋
	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* LookUpIA; // 마우스 Y 인풋
	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* TurnIA; // 마우스 X 인풋
	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* ZoomIA; // 마우스 휠 인풋
	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* SearchIA; // R키 인풋

	///////////// Camera /////////////

	UPROPERTY(EditAnywhere,Category="Camera")
	class UCameraComponent* CameraComponent; // 카메라 컴포넌트

	UPROPERTY(EditAnywhere,Category="Camera")
	class USpringArmComponent* CameraBoom; // 스프링 암 컴포넌트
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	bool bIsCameraMode; // 카메라 모드

	UPROPERTY(EditAnywhere, Category="Camera")
	float LookInterpSpeed = 10.0f; // LookUp 보간 속도

	UPROPERTY(EditAnywhere, Category="Camera")
	float TurnInterpSpeed = 10.0f; // Turn 보간 속도

	UPROPERTY(EditAnywhere, Category="Camera")
	float CameraDamping = 5.0f;  // 감속 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float CameraZoomSpeed = 10.0f; // 휠 조작 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MinCameraHeight = 250.0f; // 최소 카메라 높이

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MaxCameraHeight = 600.0f; // 최대 카메라 높이
	
	///////////// Init Function /////////////

public:

	///////////// Input /////////////

	void ToggleView(const FInputActionValue& Value); // 카메라 회전 (우클릭 누를 때)
	
	void ToggleViewStop(const FInputActionValue& Value); // 카메라 회전 (우클릭 땔 때)
	
	void LookUp(const FInputActionValue& Value); // 카메라 회전 (마우스 Y축)
	
	void Turn(const FInputActionValue& Value); // 카메라 회전 (마우스 X축)
	
	void Zoom(const FInputActionValue& Value); // 카메라 스프링 암 거리 (휠)

	void Search(const FInputActionValue& Value); // 액터 서치 (R키 누를 때)

	///////////// Action /////////////

	void SetTargetEnemy(AActor* NewTarget); // 타겟 변경

	///////////// Camera /////////////

	void ApplyCameraDamping(); // 카메라 보간 함수

};
