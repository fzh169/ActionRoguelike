// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGame.h"

FPlayerSaveData* USSaveGame::GetPlayerData(APlayerState* PlayerState)
{
	if (PlayerState == nullptr) {
		return nullptr;
	}

	// PIE�в������UniqueId���޷�ʹ��PlayerID���������������ݡ�
	if (PlayerState->GetWorld()->IsPlayInEditor()) {		// UObjects�޷�ֱ�ӷ���UWorld����˸�Ϊͨ��PlayerState��ȡ��
		if (SavedPlayers.IsValidIndex(0)) {
			return &SavedPlayers[0];		// ʹ�������еĵ�һ����Ŀ
		}
		return nullptr;
	}
	
	// GetUniqueId()��������ID��GetUniqueID()��UObject�е�һ��������
	FString PlayerID = PlayerState->GetUniqueId().ToString();
	return SavedPlayers.FindByPredicate([&](const FPlayerSaveData& Data) { return Data.PlayerID == PlayerID; });	// �������鲢��PlayerID����ƥ��
}
