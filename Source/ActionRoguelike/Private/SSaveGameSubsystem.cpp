// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGameSubsystem.h"

void USSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const USSaveGameSettings* SGSettings = GetDefault<USSaveGameSettings>();
	CurrentSlotName = SGSettings->SaveSlotName;
}

void USSaveGameSubsystem::HandleStartingNewPlayer(AController* NewPlayer)
{
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (ensure(PS)) {
		PS->LoadPlayerState(CurrentSaveGame);
	}
}

bool USSaveGameSubsystem::OverrideSpawnTransform(AController* NewPlayer)
{
	if (!IsValid(NewPlayer)) {
		return false;
	}

	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (PS == nullptr) {
		return false;
	}

	if (APawn* MyPawn = PS->GetPawn()) {
		FPlayerSaveData* FoundData = CurrentSaveGame->GetPlayerData(PS);
		if (FoundData && FoundData->bResumeAtTransform) {
			MyPawn->SetActorLocation(FoundData->Location);
			MyPawn->SetActorRotation(FoundData->Rotation);

			AController* PC = Cast<AController>(PS->GetOwner());
			PC->SetControlRotation(FoundData->Rotation);
			return true;
		}
	}
	return false;
}

void USSaveGameSubsystem::SetSlotName(FString NewSlotName)
{
	if (NewSlotName.Len() == 0) {
		return;
	}
	CurrentSlotName = NewSlotName;
}

void USSaveGameSubsystem::WriteSaveGame()
{
	CurrentSaveGame->SavedPlayers.Empty();
	CurrentSaveGame->SavedActors.Empty();

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS == nullptr) {
		return;
	}

	for (int32 i = 0; i < GS->PlayerArray.Num(); ++i) {
		ASPlayerState* PS = Cast<ASPlayerState>(GS->PlayerArray[i]);
		if (PS) {
			PS->SavePlayerState(CurrentSaveGame);
			break;		// 单人游戏
		}
	}

	for (FActorIterator It(GetWorld()); It; ++It) {
		AActor* Actor = *It;
		// 注意：可以为要保存的Actor使用专用的SavableObject接口，而不是复用GameplayInterface。
		if (!IsValid(Actor) || !Actor->Implements<USGameplayInterface>()) {		// 跳过正在被摧毁的Actor
			continue;
		}
		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetFName();
		ActorData.Transform = Actor->GetTransform();

		FMemoryWriter MemWriter(ActorData.ByteData);
		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		Ar.ArIsSaveGame = true;			// 只寻找被标记为UPROPERTY(SaveGame)的变量
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);
	OnSaveGameWritten.Broadcast(CurrentSaveGame);
}

void USSaveGameSubsystem::LoadSaveGame(FString InSlotName /*= ""*/)
{
	SetSlotName(InSlotName);

	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0)) {
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
		if (CurrentSaveGame == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data."));
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));
		for (FActorIterator It(GetWorld()); It; ++It) {
			AActor* Actor = *It;
			if (!Actor->Implements<USGameplayInterface>()) {
				continue;
			}
			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors) {
				if (ActorData.ActorName == Actor->GetFName()) {
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemReader(ActorData.ByteData);
					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					Ar.ArIsSaveGame = true;
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}
		}
		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	}
	else {
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("Created New SaveGame Data."));
	}
}
