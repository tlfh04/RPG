// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Enemy.h"

#include "Base_Player.h"
#include "EnemyStateComponent.h"
#include "TargetEnemyWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABase_Enemy::ABase_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StateUIWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StateUI"));
	StateUIWidgetComponent->SetupAttachment(RootComponent);

	StateUIWidgetComponent->SetRelativeLocation(FVector(0.f,0.f,220.f));
	StateUIWidgetComponent->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	StateUIWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	StateUIWidgetComponent->SetDrawSize(FVector2d(100.f,50.f));
	
}

// Called when the game starts or when spawned
void ABase_Enemy::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Enemy"));

	StateUIWidgetComponent->SetVisibility(false);
	
	if (UUserWidget* RawWidget = StateUIWidgetComponent->GetUserWidgetObject())
	{
		if (UTargetEnemyWidget* EnemyUI = Cast<UTargetEnemyWidget>(RawWidget))
		{
			if (UEnemyStateComponent* State = FindComponentByClass<UEnemyStateComponent>())
			{
				EnemyUI->UpdateTargetInfo(State->GetCurrentHP(), State->GetMaxHP());
				EnemyUI->SetEnemyName(State->GetEnemyName());
			}
		}
	}
	
}

// Called every frame
void ABase_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABase_Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABase_Enemy::SetOverlayMaterial(bool bEnable)
{
	USkeletalMeshComponent* MeshComp = FindComponentByClass<USkeletalMeshComponent>();  
	if (!MeshComp) 
	{
		UE_LOG(LogTemp, Error, TEXT("ABase_Enemy::SetOverlayMaterial() - MeshComponent를 찾을 수 없습니다!"));
		return;
	}

	if (bEnable)
	{
		// Overlay Material 적용
		MeshComp->SetOverlayMaterial(OverlayMaterial);
	}
	else
	{
		// 기본 머티리얼로 되돌림
		MeshComp->SetOverlayMaterial(DefaultMaterial);
	}
}

float ABase_Enemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, class AActor* DamageCauser)
{
	UEnemyStateComponent* State = FindComponentByClass<UEnemyStateComponent>();
	if (!State)
	{
		UE_LOG(LogTemp, Error, TEXT("스테이트 컴포넌트 없음"));
		return 0.f;
	}

	State->TakeDamage(DamageAmount);

	if (UUserWidget* RawWidget = StateUIWidgetComponent->GetUserWidgetObject())
	{
		if (UTargetEnemyWidget* EnemyUI = Cast<UTargetEnemyWidget>(RawWidget))
		{
			{
				EnemyUI->UpdateTargetInfo(State->GetCurrentHP(), State->GetMaxHP());
			}
		}
	}
	
	// 파티클 실행
	if (HitEffect)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 80.f); // 적 중심보다 약간 위로
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, GetActorLocation());
	}
	
	// 몽타주 실행
	if (!bIsAttacking && HitReactMontage && GetMesh() && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(HitReactMontage))
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage);
	}

	// 죽음
	if (State->GetCurrentHP() <= 0.f)
	{
		Die();
	}
	
	return DamageAmount;
}

void ABase_Enemy::StateVisible()
{
	StateUIWidgetComponent->SetVisibility(true);
}

void ABase_Enemy::StateHidden()
{
	StateUIWidgetComponent->SetVisibility(false);
}

void ABase_Enemy::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	// 애니메이션 정지
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		Anim->StopAllMontages(0.2f);
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 랙돌 활성화 (시뮬 피직스 켜서 흐물흐물하게)
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp)
	{
		MeshComp->SetSimulatePhysics(true);
		
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		// 콜리전 세팅
		// MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		// MeshComp->SetCollisionResponseToChannel(ECC_WorldStatic,   ECR_Block);
		// MeshComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		MeshComp->SetCollisionResponseToChannel(ECC_Pawn,   ECR_Ignore);
		// MeshComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
		// MeshComp->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
		// MeshComp->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
		
		// 플레이어 반대 방향으로 임펄스 주기
		if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
		{
			FVector Dir = (GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal();
			Dir.Z = 0;  // Z축 제거
			float LaunchStrength = 2800.f;  // 원하는 세기로 조절
			MeshComp->AddImpulse(Dir * LaunchStrength, TEXT("spline_02"), true);
		}
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->DisableMovement();
	}

	if (ABase_Player* Player = Cast<ABase_Player>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		if (Player->GetCurrentTarget() == this)
		{
			Player->CurrentTarget = nullptr;
			Player->ChangeState(EPlayerState::Fighting);
		}
		if  (Player->bIsQuest)
		{
			Player->UpdateCount();
		}
	}
	SetOverlayMaterial(false);
	StateHidden();

	// 일정 시간 뒤 사라짐
	SetLifeSpan(5.0f);
}

void ABase_Enemy::StateUpdate()
{
	if (UUserWidget* RawWidget = StateUIWidgetComponent->GetUserWidgetObject())
	{
		if (UTargetEnemyWidget* EnemyUI = Cast<UTargetEnemyWidget>(RawWidget))
		{
			if (UEnemyStateComponent* State = FindComponentByClass<UEnemyStateComponent>())
			{
				EnemyUI->UpdateTargetInfo(State->GetCurrentHP(), State->GetMaxHP());
				EnemyUI->SetEnemyName(State->GetEnemyName());
			}
		}
	}
}


