// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "AI/SAICharacter.h"
#include "SAttributeComponent.h"
#include "EngineUtils.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "SSaveGame.h"
#include "GameFramework/GameStateBase.h"
#include "SGameplayInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SMonsterData.h"
#include "SActionComponent.h"
#include "Engine/AssetManager.h"
#include "../ActionRoguelike.h"
#include "SSaveGameSubsystem.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("fm.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	RespawnDelay = 10.0f;
	CreditsPerKill = 20;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000.0f;
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	SG->LoadSaveGame(SelectedSaveSlot);
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);

	if (ensure(PowerupClasses.Num() > 0)) {
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance)) {
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPowerupSpawnQueryCompleted);
		}
	}
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	SG->HandleStartingNewPlayer(NewPlayer);

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);		// 将调用PlayerController中的BeginPlayingState()

	SG->OverrideSpawnTransform(NewPlayer);		// 覆盖玩家生成位置

	// 或者：跳过整个“查找玩家启动”逻辑，立即使用存储位置覆盖生成位置
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	if (!CVarSpawnBots.GetValueOnGameThread()) {

		UE_LOG(LogTemp, Warning, TEXT("Bot Spawning disabled via cvar 'CVarSpawnBots'."))
		return;
	}

	int32 NrOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It) {

		ASAICharacter* Bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributeComp(Bot);

		if (ensure(AttributeComp) && AttributeComp->IsAlive()) {

			++NrOfAliveBots;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NrOfAliveBots);

	float MaxBotCount = 5.0f;

	if (DifficultyCurve) {

		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	if (NrOfAliveBots >= MaxBotCount) {

		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity."));
		return;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if (ensure(QueryInstance)) {

		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotSpawnQueryCompleted);
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* controller)
{
	if (ensure(controller)) {

		controller->UnPossess();

		RestartPlayer(controller);
	}
}

void ASGameModeBase::OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success) {

		UE_LOG(LogTemp, Warning, TEXT("Spawn Bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.IsValidIndex(0)) {
		if (MonsterTable) {
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterInfoRow* SelectedRow = Rows[RandomIndex];

			UAssetManager* Manager = UAssetManager::GetIfValid();
			if (Manager) {
				LogOnScreen(this, FString::Printf(TEXT("Loading Monster.")));
				TArray<FName> Bundles;		// 同时加载的资产
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterID, Locations[0]);
				Manager->LoadPrimaryAsset(SelectedRow->MonsterID, Bundles, Delegate);
			}
		}
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	LogOnScreen(this, FString::Printf(TEXT("Finished Loading.")));
	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager) {
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadedId));
		if (MonsterData) {
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
			if (NewBot) {
				UE_LOG(LogTemp, Log, TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData));
				USActionComponent* ActionComp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));
				if (ActionComp) {
					for (TSubclassOf<USAction> ActionClass : MonsterData->Actions) {
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
			}
		}
	}
}

void ASGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success) {

		UE_LOG(LogTemp, Warning, TEXT("Spawn Powerup EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	TArray<FVector> UsedLocations;

	int32 SpawnCounter = 0;
	while (SpawnCounter < DesiredPowerupCount && Locations.Num()>0) {
		
		int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);
		FVector PickedLocation = Locations[RandomLocationIndex];
		Locations.RemoveAt(RandomLocationIndex);

		bool bValidLocation = true;
		for (FVector OtherLocation : UsedLocations) {
			if ((PickedLocation - OtherLocation).Size() < RequiredPowerupDistance) {
				bValidLocation = false;
				break;
			}
		}

		if (bValidLocation) {
			int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
			TSubclassOf<AActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];

			GetWorld()->SpawnActor<AActor>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);

			UsedLocations.Add(PickedLocation);
			++SpawnCounter;
		}
	}

}

void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It) {

		ASAICharacter* Bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributeComp(Bot);

		if (ensure(AttributeComp) && AttributeComp->IsAlive()) {

			AttributeComp->Kill(this);		// 或者输入Player
		}
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if (Player) {
		FTimerHandle TimerHandle_RespawnDelay;

		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}

	APawn* KillerPawn = Cast<APawn>(Killer);
	if (KillerPawn) {

		ASPlayerState* PS = KillerPawn->GetPlayerState<ASPlayerState>();
		if (PS) {

			PS->AddCredits(CreditsPerKill);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
}
