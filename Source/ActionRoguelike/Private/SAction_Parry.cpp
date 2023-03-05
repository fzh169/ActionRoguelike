// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_Parry.h"
#include "GameFramework/Character.h"

USAction_Parry::USAction_Parry()
{
	ParryDelay = 2.0f;
}

void USAction_Parry::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);

	if (Character) {
		Character->PlayAnimMontage(ParryAnim);

		FTimerHandle TimerHandle_ParryDelay;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StopAction", Instigator);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ParryDelay, Delegate, ParryDelay, false);
	}
}
