// Fill out your copyright notice in the Description page of Project Settings.


#include "RetractionNotifyState.h"
#include "Base_Player.h"

void URetractionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ABase_Player* Player = Cast<ABase_Player>(MeshComp->GetOwner());
	if (Player)
	{
		Player->ChangeState(EPlayerState::Idle);  // Idle 상태로 변경
	}
}

void URetractionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ABase_Player* Player = Cast<ABase_Player>(MeshComp->GetOwner());
	if (Player)
	{
		Player->AttachWeaponToSocket(TEXT("spine_05Socket"));  // 무기를 등으로 이동
	}
}
