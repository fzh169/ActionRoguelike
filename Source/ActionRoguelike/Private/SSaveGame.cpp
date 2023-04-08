// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGame.h"

FPlayerSaveData* USSaveGame::GetPlayerData(APlayerState* PlayerState)
{
	if (PlayerState == nullptr) {
		return nullptr;
	}

	// PIE中不会给出UniqueId，无法使用PlayerID检索保存的玩家数据。
	if (PlayerState->GetWorld()->IsPlayInEditor()) {		// UObjects无法直接访问UWorld，因此改为通过PlayerState获取。
		if (SavedPlayers.IsValidIndex(0)) {
			return &SavedPlayers[0];		// 使用数组中的第一个条目
		}
		return nullptr;
	}
	
	// GetUniqueId()返回在线ID，GetUniqueID()是UObject中的一个函数。
	FString PlayerID = PlayerState->GetUniqueId().ToString();
	return SavedPlayers.FindByPredicate([&](const FPlayerSaveData& Data) { return Data.PlayerID == PlayerID; });	// 迭代数组并按PlayerID进行匹配
}
