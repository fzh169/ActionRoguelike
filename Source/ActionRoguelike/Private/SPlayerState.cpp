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

bool ASPlayerState::UpdatePersonalRecord(float NewTime)
{
	if (NewTime > PersonalRecordTime) {
		float OldRecord = PersonalRecordTime;
		PersonalRecordTime = NewTime;
		OnRecordTimeChanged.Broadcast(this, PersonalRecordTime, OldRecord);
		return true;
	}
	return false;
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject) {
		FPlayerSaveData SaveData;
		SaveData.Credits = Credits;
		SaveData.PersonalRecordTime = PersonalRecordTime;
		SaveData.PlayerID = GetUniqueId().ToString();

		// 保存时可能不存在(Not Alive)
		if (APawn* MyPawn = GetPawn()) {
			SaveData.Location = MyPawn->GetActorLocation();
			SaveData.Rotation = MyPawn->GetActorRotation();
			SaveData.bResumeAtTransform = true;
		}

		SaveObject->SavedPlayers.Add(SaveData);
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject) {
		FPlayerSaveData* FoundData = SaveObject->GetPlayerData(this);
		if (FoundData) {
			AddCredits(FoundData->Credits);		// 触发OnCreditsChanged
			PersonalRecordTime = FoundData->PersonalRecordTime;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Could not find SaveGame data for player id '%i'."), GetPlayerId());
		}
	}
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}