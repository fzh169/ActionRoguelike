// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class APlayerState;
class USSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRecordTimeChanged, ASPlayerState*, PlayerState, float, NewTime, float, OldRecord);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = "OnRep_Credits", Category = "Credits")
	int32 Credits;

	UPROPERTY(BlueprintReadOnly)
	float PersonalRecordTime;

	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);	// OnRep_����ʹ�ð�����󶨵��ı�����ֵ�Ĳ��������ڼ���delta��

	// ����ʹ�öಥ��ȱ������Ҫͨ�����緢�͸�������ݡ�
	// OnRep_���ǡ���ѵġ�����Ϊ��ֵ����ͨ�����紫�ݣ���Credits�������Ҫ���ơ�
	// UFUNCTION(NetMulticast, Unreliable)
	// void MulticastCredits(float NewCredits, float Delta);		// ��������������RPC

public:

	UFUNCTION(BlueprintCallable, Category = "Credits")
	int32 GetCredits() const;
	
	UFUNCTION(BlueprintCallable, Category = "Credits")
	void AddCredits(int32 Delta);
	
	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool UseCredits(int32 Delta);

	UFUNCTION(BlueprintCallable)
	bool UpdatePersonalRecord(float NewTime);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRecordTimeChanged OnRecordTimeChanged;

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);
};
