// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackComponent.h"
#include "Base_Enemy.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "CombatInterface.h"
#include "Base_Player.generated.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Idle       UMETA(DisplayName = "Idle"),
	Moving     UMETA(DisplayName = "Moving"),
	Fighting  UMETA(DisplayName = "Fighting"),
	Attacking  UMETA(DisplayName = "Attacking"),
	Dead       UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None	UMETA(DisplayName = "None"),
	Basic	UMETA(DisplayName = "BasicAttack"),
	Slash	UMETA(DisplayName = "Slash"),
	Dash	UMETA(DisplayName = "Dash")
};

// Forward Declaration
class UInputMappingContext;
class UInputAction;

UCLASS()
class RPG_API ABase_Player : public ACharacter, public ICombatInterface
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
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Target")
	AActor* CurrentTarget; // 현재 타겟

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPlayerState CurrentState; // 현재 스테이트
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EAttackType CurrentAttackType = EAttackType::None; // 스킬 스테이트
	
	FVector PendingMoveLocation; // 공격 중에 누른 위치 저장용

	bool bHasPendingMove = false; // 예약 이동 체크

	UPROPERTY(EditAnywhere,Category="Target")
	AActor* PendingTarget = nullptr; // 예약 액터 저장

	UPROPERTY(EditAnywhere,Category="Component")
	UAttackComponent* AttackComponent; // 어택 컴포넌트

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsCombat = false; // 전투중인가?

	float AnimPlayRate = 1.5f; // 몽타주 실행 속도

	UPROPERTY(EditDefaultsOnly, Category="NPC")
	float NPCInteractRange = 100.f; // NPC Interact 거리
	
	UPROPERTY()
	AActor* DashTarget = nullptr;

	UPROPERTY()
	AActor* NPCTarget = nullptr;

	UPROPERTY(VisibleInstanceOnly,Category="Search")
	TArray<AActor*> NPCTargets; // NPC 타겟

	UPROPERTY(VisibleInstanceOnly,Category="Search")
	TArray<AActor*> TriggerBoxs; // TriggerBox 엑터 저장

	bool bIsQuest = false; // 퀘스트 중인가?

	int32 QuestCurrentCount = 0; // 현재 퀘스트 카운트

	int32 QuestTotalCount = 10; // 총 퀘스트 카운트

	bool bIsAutoAttacking = false; // 자동공격 중인가?

	bool bIsAttackMontagePlaying = false; // 몽타주 실행중인가?

	TWeakObjectPtr<ABase_Enemy> AutoTarget; // 자동공격 타겟

	///////////// Combat /////////////

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	float MaxHP;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Combat")
	float CurrentHP;
	
	UPROPERTY(EditDefaultsOnly, Category="Basic Attack")
	float BasicAttackRange = 100.f; // 기본 공격 사거리
	
	int32 CurrentComboIndex = 0; // 콤보 인덱스 초기화
	
	int32 MaxComboCount = 3; // 최대 콤보 인덱스 초기화
	
	UPROPERTY(EditDefaultsOnly, Category="Dash Attack")
	float DashAttackRange = 350.0f; // 대쉬 거리

	UPROPERTY(EditDefaultsOnly, Category="Dash Attack")
	float DashDuration = 0.38f; // 대쉬가 도착까지 걸리는 시간

	UPROPERTY(EditDefaultsOnly, Category="Dash Attack")
	float DashDamage = 50.0f; // 대쉬 데미지
	
	UPROPERTY(EditDefaultsOnly, Category = "Dash Attack")
	float DashCooldown = 10.0f; // 대쉬 쿨다운

	bool bDashOnCooldown = false; // 대쉬 쿨다운 여부

	bool bIsDash = false; // 대쉬중인가?

	UPROPERTY(EditDefaultsOnly, Category = "Slash Attack")
	float SlashCooldown = 6.0f; // 슬래쉬 쿨다운

	bool bSlashOnCooldown = false; // 슬래쉬 쿨다운 여부

	bool bIsAttacking = false; // 공격중인가?

	bool bIsHitting = false; // 맞는중인가?
	
	///////////// Handle /////////////
	
	FTimerHandle CameraDampingTimerHandle; // 카메라 보간 타이머
	FTimerHandle FightTimerHandle;  // 전투 상태 유지 타이머
	FTimerHandle ComboResetHandle; // 콤보 리셋 타이머
	FTimerHandle RangeCheckHandle; // 사거리 체크 타이머
	FTimerHandle NPCRangeCheckHandle; // NPC 사거리 체크 타이머
	FTimerHandle DashCooldownTimerHandle; // 대쉬 쿨다운 타이머
	FTimerHandle DashResetTimerHandle; // 대쉬 중력 해제 타이머
	FTimerHandle SlashCooldownTimerHandle; // 슬래쉬 쿨다운 타이머
	FTimerHandle AutoAttackHandle; // 자동공격 타이머
	FTimerHandle NextAttackHandle; // 다음 타겟 지정 타이머
public:

	///////////// Input /////////////

	UPROPERTY(EditAnywhere,Category="Input")
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* ToggleViewIA; // 우클릭 인풋
	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* MouseMoveIA; // 좌클릭 인풋
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

	///////////// Animation /////////////

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
    TArray<UAnimMontage*> BasicAttackMontages;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* DashAttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* SlashAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* RetractionMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HitReactMontage;
	
	///////////// Mesh /////////////

	UPROPERTY(EditAnywhere, Category= "Mesh")
	UStaticMeshComponent* WeaponMesh;
	
	///////////// Init Function /////////////

public:

	///////////// Input /////////////

	void ToggleView(const FInputActionValue& Value); // 카메라 회전 (우클릭 누를 때)
	
	void ToggleViewStop(const FInputActionValue& Value); // 카메라 회전 (우클릭 땔 때)

	void MouseMove(const FInputActionValue& Value); // 마우스 이동 (죄클릭)
	
	void LookUp(const FInputActionValue& Value); // 카메라 회전 (마우스 Y축)
	
	void Turn(const FInputActionValue& Value); // 카메라 회전 (마우스 X축)
	
	void Zoom(const FInputActionValue& Value); // 카메라 스프링 암 거리 (휠)

	void Search(const FInputActionValue& Value); // 액터 서치 (R키 누를 때)

	///////////// Action /////////////
	
	UFUNCTION(BlueprintCallable)
	void SetTargetEnemy(AActor* NewTarget); // 타겟 변경
	
	UFUNCTION(BlueprintCallable, Category = "FSM")
	void ChangeState(EPlayerState NewState); // State 변경
	
	UFUNCTION(BlueprintCallable,  Category = "FSM")
	void HandleState(); // State 핸들
	
	void AttachWeaponToSocket(FName SocketName);  // 무기 이동 함수

	void ReturnToIdleState(); // Idle 전환 함수

	void FaceTarget(AActor* Target); // 적 쳐다보기

	void MoveToNpc(); // NPC에게 다가가기

	void MoveToTriggerBox(); // 트리거 박스로 이동

	void OverlapTriggerBox();

	///////////// Combat /////////////
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
						 class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void TryBasicAttack(); // 공격 시도 함수

	UFUNCTION()
	void CheckRangeAndAttack(); // 공격 범위 체크 함수

	UFUNCTION()
	void PlayBasicAttackMontage(); // 기본 공격 몽타주 함수
	
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted); // 몽타주 끝난 후 함수

	UFUNCTION()
	void DashAttack(AActor* Target); // 대쉬 공격 몽타주 함수

	UFUNCTION()
	void SlashAttack(); // 슬래쉬 공격 몽타주 함수

	UFUNCTION()
	void StartAutoAttack(); // 자동공격 시작

	UFUNCTION()
	void StopAutoAttack(); // 자동공격 종료

	UFUNCTION()
	void AutoAttackNext(); // 다음 타겟 지정 함수

	UFUNCTION()
	void CheckAutoTargetAlive(); // 적이 죽었는지 체크 함수
	
	///////////// Camera /////////////

	void ApplyCameraDamping(); // 카메라 보간 함수
	
	///////////// Animation /////////////
	
	void PlayMontage(UAnimMontage* Montage); // 몽타주 실행

	///////////// Timer /////////////

	void StartCombatIdleTimer(); // 전투중인지 체크하는 함수

	void ResetDashCooldown(); // 대쉬 쿨다운 리셋

	void ResetSlashCooldown(); // 슬래쉬 쿨다운 리셋

	void ResetDashPhysics(); // 대쉬 중력 해제 함수

	void CheckRangeNPC(); // NPC와 가까운지 체크하는 함수

	void UpdateCount(); // 퀘스트 카운트 업데이트

	///////////// Getter /////////////

	UFUNCTION(BlueprintCallable, Category="Player State")
	EPlayerState GetCurrentState() const { return CurrentState; }
	EAttackType GetCurrentAttackType() const { return CurrentAttackType; }
	AActor* GetCurrentTarget() const { return CurrentTarget; }
};