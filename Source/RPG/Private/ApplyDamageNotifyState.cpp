// Fill out your copyright notice in the Description page of Project Settings.


#include "ApplyDamageNotifyState.h"
#include "Base_Player.h"
#include "Kismet/GameplayStatics.h"

void UApplyDamageNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AActor* Owner = MeshComp->GetOwner();
	
	if (ABase_Player* Player = Cast<ABase_Player>(Owner))
	{
		AActor* Target = Player->GetCurrentTarget();
		if (!Target) return;

		float Damage = 0.f;

		switch (Player->GetCurrentAttackType())
		{
		case EAttackType::Basic: Damage = 10.f; break;
		case EAttackType::Slash: Damage = 25.f; break;
		case EAttackType::Dash:  Damage = 40.f; break;
		default: return;
		}

		UGameplayStatics::ApplyDamage(
			Target,
			Damage,
			Player->GetController(),
			Player,
			UDamageType::StaticClass()
		);
	}
	
}
