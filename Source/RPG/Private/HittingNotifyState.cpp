// Fill out your copyright notice in the Description page of Project Settings.


#include "HittingNotifyState.h"

#include "Base_Player.h"

void UHittingNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp)
	{
		if (ABase_Player* Player = Cast<ABase_Player>(MeshComp->GetOwner()))
		{
			Player->bIsHitting = true;
		}
	}
}

void UHittingNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp)
	{
		if (ABase_Player* Player = Cast<ABase_Player>(MeshComp->GetOwner()))
		{
			Player->bIsHitting = false;
		}
	}
}
