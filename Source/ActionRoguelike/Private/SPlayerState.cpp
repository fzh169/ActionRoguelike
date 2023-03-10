// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "SSaveGame.h"

int32 ASPlayerState::GetCredits() const
{
	return Credits;
}

void ASPlayerState::AddCredits(int32 Delta)
{
	if (ensure(Delta > 0.0f)) {

		Credits += Delta;

		OnCreditsChanged.Broadcast(this, Credits, Delta);
	}
}

bool ASPlayerState::UseCredits(int32 Delta)
{
	if (ensure(Delta > 0.0f)) {

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
		Credits = SaveObject->Credits;
	}
}
