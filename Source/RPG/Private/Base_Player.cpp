// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Player.h"

#include "EnemyStateComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UIManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

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

	Tags.Add(FName("Player"));
	
}

// Called when the game starts or when spawned
void ABase_Player::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext,0);
		}
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
	for (AActor* Enemy : FoundEnemies)
	{
		if (!ClosestEnemies.Contains(Enemy)) // 중복 검사
		{
			ClosestEnemies.Add(Enemy);
		}

		if (ClosestEnemies.Num() >= 10) break;
	}

	// 가장 가까운 적들 출력
	UE_LOG(LogTemp, Warning, TEXT("적을 찾았습니다"));
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
	if (CurrentTarget)
	{
		// 기존 타겟의 머티리얼에서 테두리 효과 OFF
		UMeshComponent* OldMesh = CurrentTarget->FindComponentByClass<UMeshComponent>();
		if (OldMesh)
		{
			for (int i = 0; i < OldMesh->GetNumMaterials(); i++)
			{
				UMaterialInstanceDynamic* DynMaterial = Cast<UMaterialInstanceDynamic>(OldMesh->GetMaterial(i));
				if (DynMaterial)
				{
					DynMaterial->SetScalarParameterValue(FName("OutlineEnabled"), 0.0f);
				}
			}
		}
	}

	CurrentTarget = NewTarget;

	if (CurrentTarget)
	{
		// 새로운 타겟의 머티리얼에서 테두리 효과 ON
		UMeshComponent* NewMesh = CurrentTarget->FindComponentByClass<UMeshComponent>();
		if (NewMesh)
		{
			for (int i = 0; i < NewMesh->GetNumMaterials(); i++)
			{
				UMaterialInstanceDynamic* DynMaterial = NewMesh->CreateAndSetMaterialInstanceDynamic(i);
				if (DynMaterial)
				{
					DynMaterial->SetScalarParameterValue(FName("OutlineEnabled"), 1.0f);
				}
			}
		}
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


