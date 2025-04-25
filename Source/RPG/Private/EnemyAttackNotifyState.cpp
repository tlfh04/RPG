// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAttackNotifyState.h"
#include "Base_Player.h"
#include "Base_Enemy.h"

void UEnemyAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp)
	{
		if (ABase_Enemy* Enemy = Cast<ABase_Enemy>(MeshComp->GetOwner()))
		{
			Enemy->bIsAttacking = true;
		}
	}
	if (MeshComp)
	{
		if (ABase_Player* Player = Cast<ABase_Player>(MeshComp->GetOwner()))
		{
			Player->bIsAttacking = true;
		}
	}
}

void UEnemyAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp)
	{
		if (ABase_Enemy* Enemy = Cast<ABase_Enemy>(MeshComp->GetOwner()))
		{
			Enemy->bIsAttacking = false;
		}
	}
	if (MeshComp)
	{
		if (ABase_Player* Player = Cast<ABase_Player>(MeshComp->GetOwner()))
		{
			Player->bIsAttacking = false;
		}
	}
}
