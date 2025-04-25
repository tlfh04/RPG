// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Player.h"

#include "AttackComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Base_Enemy.h"
#include "EnemyStateComponent.h"
#include "UIManager.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/CameraComponent.h"
#include "Engine/TriggerBox.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

// Forward Declaration

// Sets default values
ABase_Player::ABase_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스프링암 (SpringArm) 생성 및 설정
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = MaxCameraHeight; // 3인칭 거리 설정
	CameraBoom->bUsePawnControlRotation = true; // 플레이어 컨트롤러 회전을 따라가도록 설정

	// 카메라 생성 및 설정
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(CameraBoom);
	CameraComponent->bUsePawnControlRotation = true; // 카메라는 직접 회전하지 않음

	// 기본 설정
	bUseControllerRotationYaw = false; // 기본적으로 마우스로 회전 가능
	bIsCameraMode = false; // 기본 상태는 일반 컨트롤 모드
	GetCharacterMovement()->bOrientRotationToMovement = true;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh")); // 무기 메쉬
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("spine_05Socket")); // 등에 부착
	AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("AttackComponent")); // 어택 컴포넌트

	Tags.Add(FName("Player"));
	
}

// Called when the game starts or when spawned
void ABase_Player::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext,0);
			
		}
	}

	UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>();
	if (UIManager)
	{
		UIManager->InitializeUI(GetWorld());
	}
}

// Called every frame
void ABase_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void ABase_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ToggleViewIA,ETriggerEvent::Triggered,this,&ABase_Player::ToggleView);
		EnhancedInputComponent->BindAction(ToggleViewIA,ETriggerEvent::Completed,this,&ABase_Player::ToggleViewStop);
		EnhancedInputComponent->BindAction(MouseMoveIA,ETriggerEvent::Completed,this,&ABase_Player::MouseMove);
		EnhancedInputComponent->BindAction(LookUpIA,ETriggerEvent::Triggered,this,&ABase_Player::LookUp);
		EnhancedInputComponent->BindAction(TurnIA,ETriggerEvent::Triggered,this,&ABase_Player::Turn);
		EnhancedInputComponent->BindAction(ZoomIA,ETriggerEvent::Triggered,this,&ABase_Player::Zoom);
		EnhancedInputComponent->BindAction(SearchIA,ETriggerEvent::Started,this,&ABase_Player::Search);
	}
}

void ABase_Player::ToggleView(const FInputActionValue& Value)
{
	bool bIsPressed = Value.Get<bool>();
	
	bIsCameraMode = bIsPressed;

	if (bIsCameraMode)
	{
		// 우클릭을 누르면 타이머 중지
		GetWorld()->GetTimerManager().ClearTimer(CameraDampingTimerHandle);
	}
	
}

void ABase_Player::ToggleViewStop(const FInputActionValue& Value)
{
	bool bIsPressed = Value.Get<bool>();
	
	bIsCameraMode = bIsPressed;
	
	if (!bIsCameraMode)
	{
		// 우클릭을 놓았을 때 감속을 시작하는 타이머 실행
		GetWorld()->GetTimerManager().SetTimer(CameraDampingTimerHandle, this, &ABase_Player::ApplyCameraDamping, 0.016f, true);
	}
	
}

void ABase_Player::MouseMove(const FInputActionValue& Value)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FVector WorldLocation;
	FVector WorldDirection;

	if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		FVector TraceStart = WorldLocation;
		FVector TraceEnd = TraceStart + (WorldDirection * 10000.f);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
		{
			AActor* HitActor = Hit.GetActor();
			NPCTarget = HitActor;
			if (HitActor && HitActor->ActorHasTag("NPC"))
			{
				ChangeState(EPlayerState::Moving);
				UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), NPCTarget);

				UE_LOG(LogTemp, Warning, TEXT("npc 쪽으로 이동중"));

				GetWorldTimerManager().ClearTimer(NPCRangeCheckHandle);
				GetWorldTimerManager().SetTimer(NPCRangeCheckHandle, this, &ABase_Player::CheckRangeNPC, 0.2f, true);
			}

			if (HitActor && HitActor->ActorHasTag("Enemy"))
			{
				SetTargetEnemy(HitActor);
			}
			else
			{
				SetTargetEnemy(nullptr);
				
				if (CurrentState == EPlayerState::Attacking)
				{
					// 공격 중이면 위치만 저장
					PendingMoveLocation = Hit.ImpactPoint;
					bHasPendingMove = true;
				}
				else
				{
					// 공격 중이 아니면 즉시 이동
					ChangeState(EPlayerState::Moving);
					UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Hit.ImpactPoint);
				}
			}
		}
	}
}

void ABase_Player::LookUp(const FInputActionValue& Value)
{
	if (!bIsCameraMode) return;
	
	const float InputValue = Value.Get<float>();
	CurrentPitchSpeed = InputValue * 2.0f;  // 민감도 조절

	FRotator NewRotation = GetControlRotation();
	NewRotation.Pitch += CurrentPitchSpeed;
	GetController()->SetControlRotation(NewRotation);
	
}

void ABase_Player::Turn(const FInputActionValue& Value)
{
	if (!bIsCameraMode) return;
	
	const float InputValue = Value.Get<float>();
	CurrentYawSpeed = InputValue * 2.0f;

	FRotator NewRotation = GetControlRotation();
	NewRotation.Yaw += CurrentYawSpeed;
	GetController()->SetControlRotation(NewRotation);
	
}

void ABase_Player::Zoom(const FInputActionValue& Value)
{
	// 휠 스크롤 입력 값 가져오기
	const float ScrollValue = Value.Get<float>();

	// 현재 카메라 높이를 입력 값에 따라 변경
	float NewArmLength = CameraBoom->TargetArmLength - (ScrollValue * CameraZoomSpeed);

	// 최소/최대 높이 제한 적용
	NewArmLength = FMath::Clamp(NewArmLength, MinCameraHeight, MaxCameraHeight);

	// 변경된 값 적용
	CameraBoom->TargetArmLength = NewArmLength;

	UE_LOG(LogTemp, Warning, TEXT("New Camera Height: %f"), NewArmLength);
	
}

void ABase_Player::Search(const FInputActionValue& Value)
{

	// 배열 초기화 (기존 데이터 제거)
	FoundEnemies.Empty();
	ClosestEnemies.Empty();

	// "Enemy" 태그가 있는 모든 액터 찾기
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundEnemies);

	if (FoundEnemies.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("적을 찾을 수 없습니다."));
		return;
	}

	// 플레이어 위치 가져오기
	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	// 적들을 거리 기준으로 정렬
	FoundEnemies.Sort([PlayerLocation](const AActor& A, const AActor& B)
	{
		return FVector::Dist(PlayerLocation, A.GetActorLocation()) < FVector::Dist(PlayerLocation, B.GetActorLocation());
	});

	// 중복 없는 가장 가까운 10마리의 적 저장
	for (AActor* Actor : FoundEnemies)
	{
		ABase_Enemy* Enemy = Cast<ABase_Enemy>(Actor);
		if (!IsValid(Enemy) || Enemy->IsDead()) continue;
		
		if (!ClosestEnemies.Contains(Enemy)) // 중복 검사
		{
			ClosestEnemies.Add(Enemy);
		}

		if (ClosestEnemies.Num() >= 10) break;
	}

	// 가장 가까운 적들 출력
	for (int i = 0; i < ClosestEnemies.Num(); i++)
	{
		UEnemyStateComponent* EnemyState = ClosestEnemies[i]->FindComponentByClass<UEnemyStateComponent>();
		// nullptr 체크
		FString ClosestEnemyName = EnemyState ? EnemyState->GetEnemyName() : TEXT("Unknown Enemy");
		
		float Distance = FVector::Dist(PlayerLocation, ClosestEnemies[i]->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("%d. %s"), i + 1, *ClosestEnemyName);
	}

	// UI 업데이트
	UUIManager* UIManager = UUIManager::GetInstance(this);
	UE_LOG(LogTemp, Warning, TEXT("UI 인스턴스 생성 및 초기화"));
	
	if (UIManager)
	{
		UIManager->UpdateEnemyList(ClosestEnemies);
	}
}

void ABase_Player::SetTargetEnemy(AActor* NewTarget)
{
	if (!NewTarget) return;

	if (ABase_Enemy* Enemy = Cast<ABase_Enemy>(NewTarget))
	{
		if (Enemy->IsDead()) return;
	}
	
	if (CurrentState == EPlayerState::Attacking)
	{
		PendingTarget = NewTarget;
		
		if (ABase_Enemy* OldEnemy = Cast<ABase_Enemy>(CurrentTarget))
		{
			OldEnemy->SetOverlayMaterial(false);
			OldEnemy->StateHidden();
		}

		if (ABase_Enemy* NewEnemy = Cast<ABase_Enemy>(NewTarget))
		{
			NewEnemy->SetOverlayMaterial(true);
			NewEnemy->StateVisible();
		}
		
		return;
	}

	// 기존 타겟이 있다면 Overlay Material 비활성화
	if (CurrentTarget)
	{
		ABase_Enemy* OldEnemy = Cast<ABase_Enemy>(CurrentTarget);
		if (OldEnemy)
		{
			OldEnemy->SetOverlayMaterial(false);
			OldEnemy->StateHidden();
		}
	}

	// 새로운 타겟 설정
	CurrentTarget = NewTarget;

	ABase_Enemy* NewEnemy = Cast<ABase_Enemy>(NewTarget);
	if (NewEnemy)
	{
		NewEnemy->SetOverlayMaterial(true);  // Overlay Material 활성화
		NewEnemy->StateVisible();
		if (CurrentState != EPlayerState::Attacking)
		{
			TryBasicAttack();
			ChangeState(EPlayerState::Fighting);
		}
		
		StartCombatIdleTimer();
	}
	
}

void ABase_Player::ChangeState(EPlayerState NewState)
{
	if (CurrentState == NewState) return;

	UE_LOG(LogTemp, Warning, TEXT("플레이어 상태 변경: %d -> %d"), (int32)CurrentState, (int32)NewState);
	
	CurrentState = NewState;
	HandleState();
	
}

void ABase_Player::HandleState()
{
	
	switch (CurrentState)
	{
	case EPlayerState::Idle:
		
		UE_LOG(LogTemp, Warning, TEXT("플레이어가 Idle 상태입니다."));

		PlayMontage(RetractionMontage);
		IsCombat = false;

		break;

	case EPlayerState::Moving:
		
		UE_LOG(LogTemp, Warning, TEXT("플레이어가 이동 중입니다."));
		CurrentComboIndex = 0;
		break;

	case EPlayerState::Fighting:

		UE_LOG(LogTemp, Warning, TEXT("플레이어가 공격 합니다."));
		
		AttachWeaponToSocket(TEXT("hand_rSocket"));  // 무기를 손으로 이동
		IsCombat = true;
		
		break;
	case EPlayerState::Attacking:

		UE_LOG(LogTemp, Warning, TEXT("플레이어가 공격 중입니다."));
		break;

	case EPlayerState::Dead:
		UE_LOG(LogTemp, Warning, TEXT("플레이어가 사망했습니다."));
		break;
	}
}

void ABase_Player::AttachWeaponToSocket(FName SocketName)
{
	if (!WeaponMesh) return;

	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	UE_LOG(LogTemp, Warning, TEXT("무기가 %s 소켓으로 이동"), *SocketName.ToString());
}

void ABase_Player::TryBasicAttack()
{
	if (CurrentState == EPlayerState::Attacking) return;
	
	if (!CurrentTarget) return;

	float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

	// 대쉬 조건
	if (!bDashOnCooldown && Distance <= DashAttackRange)
	{
		ChangeState(EPlayerState::Fighting);
		DashAttack(CurrentTarget);
		return;
	}

	if (!bSlashOnCooldown && Distance <= BasicAttackRange)
	{
		ChangeState(EPlayerState::Fighting);
		SlashAttack();
		return;
	}

	// 기본 공격 조건
	if (Distance <= BasicAttackRange)
	{
		ChangeState(EPlayerState::Fighting);
		PlayBasicAttackMontage();
		return;
	}

	// 기본공격 거리도 벗어난 경우 타겟으로 이동
	ChangeState(EPlayerState::Moving);
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentTarget);

	GetWorldTimerManager().ClearTimer(RangeCheckHandle);
	GetWorldTimerManager().SetTimer(RangeCheckHandle, this, &ABase_Player::CheckRangeAndAttack, 0.2f, true);

}

void ABase_Player::CheckRangeAndAttack()
{
	if (!CurrentTarget)
	{
		GetWorldTimerManager().ClearTimer(RangeCheckHandle);
		ChangeState(EPlayerState::Idle);
		return;
	}
	
	if (bIsDash) return;

	float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

	if (!bDashOnCooldown && Distance <= DashAttackRange)
	{
		if (AController* Ctrl = GetController()) Ctrl->StopMovement();
		ChangeState(EPlayerState::Fighting);
		DashAttack(CurrentTarget);
		GetWorldTimerManager().ClearTimer(RangeCheckHandle);
		return;
	}

	// Slash 우선
	if (!bSlashOnCooldown && Distance <= BasicAttackRange)
	{
		if (AController* Ctrl = GetController()) Ctrl->StopMovement();
		ChangeState(EPlayerState::Fighting);
		SlashAttack();
		GetWorldTimerManager().ClearTimer(RangeCheckHandle);
		return;
	}

	// Basic 공격
	if (Distance <= BasicAttackRange)
	{
		if (AController* Ctrl = GetController()) Ctrl->StopMovement();
		ChangeState(EPlayerState::Fighting);
		PlayBasicAttackMontage();
		GetWorldTimerManager().ClearTimer(RangeCheckHandle);
		return;
	}
}

void ABase_Player::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttackMontagePlaying = false; // 몽타주 끝
	CurrentAttackType = EAttackType::None; // 스킬 스테이트 초기화
	
	// 타겟 예약 처리
	if (PendingTarget)
	{
		AActor* ReservedTarget = PendingTarget;
		PendingTarget = nullptr;

		CurrentTarget = ReservedTarget;

		UE_LOG(LogTemp, Log, TEXT("예약된 타겟으로 전환: %s"), *ReservedTarget->GetName());
	}
	
	// 몽타주 끝난 후 예약된 이동 처리
	if (bHasPendingMove)
	{
		ChangeState(EPlayerState::Moving);
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PendingMoveLocation);
		bHasPendingMove = false;
		UE_LOG(LogTemp, Log, TEXT("예약된 위치로 이동 시작"));
		return;
	}
	
	if (!CurrentTarget) return;

	float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation()); // 거리 계산

	if (!bDashOnCooldown && Distance <= DashAttackRange)
	{
		ChangeState(EPlayerState::Fighting);
		TryBasicAttack(); // 계속 공격
	}
	else if (Distance <= BasicAttackRange)
	{
		ChangeState(EPlayerState::Fighting);
		TryBasicAttack(); // 계속 공격
		UE_LOG(LogTemp, Log, TEXT("여기니?"));
	}
	else
	{
		ChangeState(EPlayerState::Moving);
		TryBasicAttack(); // 사거리 밖일 경우 다시 접근
	}
}

void ABase_Player::ApplyCameraDamping()
{
	float DampingFactor = 0.9f; // 감속률 (1에 가까울수록 천천히 멈춤)

	// 현재 속도에 감속 적용
	CurrentPitchSpeed *= DampingFactor;
	CurrentYawSpeed *= DampingFactor;

	// 일정 속도 이하로 줄어들면 감속 중지
	if (FMath::Abs(CurrentPitchSpeed) < 0.01f && FMath::Abs(CurrentYawSpeed) < 0.01f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CameraDampingTimerHandle);
		return;
	}

	// 감속된 회전 값 적용
	FRotator NewRotation = GetControlRotation();
	NewRotation.Pitch += CurrentPitchSpeed;
	NewRotation.Yaw += CurrentYawSpeed;
	GetController()->SetControlRotation(NewRotation);
}

void ABase_Player::PlayMontage(UAnimMontage* Montage)
{
	if (Montage && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage);
	}
}

void ABase_Player::StartCombatIdleTimer()
{
	// 기존 타이머 제거
	if (GetWorldTimerManager().IsTimerActive(FightTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(FightTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("타이머 초기화 완료"));
	}

	// 새 타이머 설정
	GetWorldTimerManager().SetTimer(FightTimerHandle, this, &ABase_Player::ReturnToIdleState, 5.0f, false);
	UE_LOG(LogTemp, Warning, TEXT("5초 후 Idle 상태로 전환되는 타이머 설정됨"));
}

void ABase_Player::ReturnToIdleState()
{
	UE_LOG(LogTemp, Warning, TEXT("타이머 끝 Idle 상태로 변경"));
	ChangeState(EPlayerState::Idle);
}

void ABase_Player::FaceTarget(AActor* Target)
{
	if (!Target) return;
	
	FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FRotator LookAtRot = Direction.Rotation(); // 정규화된 벡터를 로테이터로
	FRotator YawRot = FRotator(0.f, LookAtRot.Yaw, 0.f); // Yaw만 회전

	SetActorRotation(YawRot);
}


void ABase_Player::PlayBasicAttackMontage()
{
	if (!CurrentTarget || !BasicAttackMontages[CurrentComboIndex] || !GetMesh()) return;
	if (bIsAttackMontagePlaying) return; // 뭉타주 실행중이면 리턴
	if (BasicAttackMontages.Num() == 0) return;

	FaceTarget(CurrentTarget);

	ChangeState(EPlayerState::Attacking); // 공격중임
	CurrentAttackType = EAttackType::Basic; // 스킬 스테이트 기본공격
	GetController()->StopMovement(); // 이동 정지

	if (CurrentComboIndex >= BasicAttackMontages.Num())
	{
		CurrentComboIndex = 0;
	}

	UAnimMontage* CurrentMontage = BasicAttackMontages[CurrentComboIndex];

	if (CurrentMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		// 몽타주 끝났을 때 호출되는 델리게이트 바인딩
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ABase_Player::OnAttackMontageEnded);
		
		AnimInstance->Montage_Play(CurrentMontage);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, CurrentMontage);
		
		StartCombatIdleTimer(); // 전투중 타이머 초기화
		
		CurrentComboIndex++;
		if (CurrentComboIndex >= MaxComboCount)
		{
			CurrentComboIndex = 0;
		}
	}
}

void ABase_Player::DashAttack(AActor* Target)
{
	if (!CurrentTarget || !DashAttackMontage || !GetMesh() ) return;
	if (bIsAttackMontagePlaying) return; // 몽타주 실행중이면 리턴

	FaceTarget(Target);
	
	ChangeState(EPlayerState::Attacking); // 공격중임
	CurrentAttackType = EAttackType::Dash; // 스킬 스테이트 대쉬공격
	GetController()->StopMovement(); // 이동 정지

	// 방향
	FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	GetCharacterMovement()->GravityScale = 0.f;
	GetCharacterMovement()->GroundFriction = 0.f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.f;
	// 런치
	LaunchCharacter(Direction * (DashAttackRange / DashDuration), true, true);

	bIsDash = true;

	GetWorldTimerManager().SetTimer(DashResetTimerHandle, this, &ABase_Player::ResetDashPhysics, DashDuration, false);
	
	UAnimMontage* CurrentMontage = DashAttackMontage;
	if (CurrentMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		// 몽타주 끝났을 때 호출되는 델리게이트 바인딩
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ABase_Player::OnAttackMontageEnded);
		
		AnimInstance->Montage_Play(CurrentMontage);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, CurrentMontage);
		
		StartCombatIdleTimer(); // 전투중 타이머 초기화

		bDashOnCooldown = true;
		GetWorldTimerManager().SetTimer(DashCooldownTimerHandle,this,&ABase_Player::ResetDashCooldown,DashCooldown,false);
	}
}

void ABase_Player::SlashAttack()
{
	if (!CurrentTarget || !SlashAttackMontage || !GetMesh()) return;
	if (bIsAttackMontagePlaying) return; // 몽타주 실행중이면 리턴

	FaceTarget(CurrentTarget);
	ChangeState(EPlayerState::Attacking);
	CurrentAttackType = EAttackType::Slash; // 스킬 스테이트 슬래쉬공격
	GetController()->StopMovement();
	
	UAnimMontage* CurrentMontage = SlashAttackMontage;
	
	if (CurrentMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		// 몽타주 끝났을 때 호출되는 델리게이트 바인딩
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ABase_Player::OnAttackMontageEnded);

		AnimInstance->Montage_Play(SlashAttackMontage);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, SlashAttackMontage);

		StartCombatIdleTimer(); // 전투중 타이머 초기화
	}

	bSlashOnCooldown = true;
	GetWorldTimerManager().SetTimer(SlashCooldownTimerHandle, this, &ABase_Player::ResetSlashCooldown, SlashCooldown, false);
}

void ABase_Player::StartAutoAttack()
{
	if (bIsAutoAttacking) return;
	bIsAutoAttacking = true;
	AutoAttackNext();
}

void ABase_Player::StopAutoAttack()
{
	bIsAutoAttacking = false;
	GetWorldTimerManager().ClearTimer(AutoAttackHandle);
	AutoTarget = nullptr;
}

void ABase_Player::AutoAttackNext()
{
	if (!bIsAutoAttacking) return;

	// 모든 Enemy 태그 액터 검색
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Found);

	// 살아 있는 Enemy 탐색 후 Sort 정렬
	Found.Sort([this](AActor& A, AActor& B){
		return FVector::Dist(GetActorLocation(), A.GetActorLocation()) < FVector::Dist(GetActorLocation(), B.GetActorLocation());
	});

	//if (UUIManager* UIManager = UUIManager::GetInstance(this))
	//{
	//	UIManager->UpdateEnemyList(Found); // 리스트에 Found 배열 전달
	//}

	// 첫 번째 유효한 타겟 찾기
	AutoTarget = nullptr;
	for (AActor* Actor : Found)
	{
		ABase_Enemy* Enemy = Cast<ABase_Enemy>(Actor);
		if (Enemy && !Enemy->IsDead())
		{
			AutoTarget = Enemy;
			break;
		}
	}

	// 없으면 끝
	if (!AutoTarget.IsValid())
	{
		StopAutoAttack();
		return;
	}

	// 타겟 설정 후 공격
	SetTargetEnemy(AutoTarget.Get());

	// 사망 체크 루프 시작 (0.2초 간격)
	GetWorldTimerManager().ClearTimer(AutoAttackHandle);
	GetWorldTimerManager().SetTimer(AutoAttackHandle,this,&ABase_Player::CheckAutoTargetAlive,0.2f,true);
}

void ABase_Player::CheckAutoTargetAlive()
{
	if (!bIsAutoAttacking)
	{
		GetWorldTimerManager().ClearTimer(AutoAttackHandle);
		return;
	}

	// 죽었거나 유효하지 않으면, 다음 타겟으로
	if (!AutoTarget.IsValid() || AutoTarget->IsDead())
	{
		GetWorldTimerManager().ClearTimer(AutoAttackHandle);
		GetWorldTimerManager().SetTimer(NextAttackHandle,this, &ABase_Player::AutoAttackNext,1.f,false);
	}
}


void ABase_Player::ResetDashCooldown()
{
	bDashOnCooldown = false;
}

void ABase_Player::ResetDashPhysics()
{
	GetCharacterMovement()->GravityScale = 1.f;
	GetCharacterMovement()->GroundFriction = 8.f;
	GetCharacterMovement()->BrakingFrictionFactor = 2.f;

	bIsDash = false;
}

void ABase_Player::CheckRangeNPC()
{
	if (!NPCTarget || !IsValid(NPCTarget))
	{
		GetWorldTimerManager().ClearTimer(NPCRangeCheckHandle);
		return;
	}

	// 현재 플레이어↔NPC 거리
	float Dist = FVector::Dist(
		GetActorLocation(),
		NPCTarget->GetActorLocation()
	);

	if (Dist <= NPCInteractRange)
	{
		GetWorldTimerManager().ClearTimer(NPCRangeCheckHandle); // 타이머 중지
		
		GetController()->StopMovement(); // 이동 정지

		// 상호작용 
		UUIManager* UIManager = UUIManager::GetInstance(this);
		UE_LOG(LogTemp, Warning, TEXT("UI 인스턴스 생성 및 초기화"));
	
		if (UIManager)
		{
			UIManager->ShowQuestProgress(QuestCurrentCount,QuestTotalCount);
		}
		
		NPCTarget = nullptr; // 타겟 초기화
	}
}

void ABase_Player::UpdateCount()
{
	QuestCurrentCount++;

	UUIManager* UIManager = UUIManager::GetInstance(this);
	UE_LOG(LogTemp, Warning, TEXT("UI 인스턴스 생성 및 초기화"));
	
	if (UIManager)
	{
		UIManager->ShowQuestProgress(QuestCurrentCount,QuestTotalCount);
	}
	
}

void ABase_Player::MoveToNpc()
{
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("NPC"), NPCTargets);

	NPCTarget = NPCTargets[0];

	ChangeState(EPlayerState::Moving);
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), NPCTarget);

	UE_LOG(LogTemp, Warning, TEXT("npc 쪽으로 이동중"));

	GetWorldTimerManager().ClearTimer(NPCRangeCheckHandle);
	GetWorldTimerManager().SetTimer(NPCRangeCheckHandle, this, &ABase_Player::CheckRangeNPC, 0.2f, true);
}

void ABase_Player::MoveToTriggerBox()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATriggerBox::StaticClass(), TriggerBoxs);

	ATriggerBox* Trigger = Cast<ATriggerBox>(TriggerBoxs[0]);

	if (Trigger)
	{
		ChangeState(EPlayerState::Moving);
		UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), Trigger);
	}
}

void ABase_Player::OverlapTriggerBox()
{
	// UI 업데이트
	UUIManager* UIManager = UUIManager::GetInstance(this);
	UE_LOG(LogTemp, Warning, TEXT("UI 인스턴스 생성 및 초기화"));
	
	if (UIManager)
	{
		UIManager->ShowQuestProgress(QuestCurrentCount,QuestTotalCount);
	}
}

void ABase_Player::ResetSlashCooldown()
{
	bSlashOnCooldown = false;
}

float ABase_Player::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{

	CurrentHP -= DamageAmount;
	ChangeState(EPlayerState::Fighting); // 전투중임
	if (!bIsHitting && !bIsAttacking && HitReactMontage && GetMesh())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage);
	}

	if (UUIManager* UIManager = UUIManager::GetInstance(this))
	{
		UIManager->UpdatePlayerHP(CurrentHP,MaxHP);
	}
	
	return DamageAmount;
}
