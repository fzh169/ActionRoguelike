// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "SSaveGame.h"
#include "Net/UnrealNetwork.h"

void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits - OldCredits);
}

/*void ASPlayerState::MulticastCredits_Implementation(float NewCredits, float Delta)
{
	OnCreditsChanged.Broadcast(this, NewCredits, Delta);
}*/

int32 ASPlayerState::GetCredits() const
{
	return Credits;
}

void ASPlayerState::AddCredits(int32 Delta)
{
	if (ensure(Delta >= 0.0f)) {

		Credits += Delta;

		OnCreditsChanged.Broadcast(this, Credits, Delta);
	}
}

bool ASPlayerState::UseCredits(int32 Delta)
{
	if (ensure(Delta >= 0.0f)) {

		if (Credits >= Delta) {

			Credits -= Delta;

			OnCreditsChanged.Broadcast(this, Credits, -Delta);

			return true;
		}
	}

	return false;
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject) {
		SaveObject->Credits = Credits;
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject) {
		AddCredits(SaveObject->Credits);		// ´¥·¢OnCreditsChanged
	}
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}