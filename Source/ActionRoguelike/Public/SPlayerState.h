// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);

class USSaveGame;

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

	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);	// OnRep_可以使用包含其绑定到的变量旧值的参数，用于计算delta。

	// 这里使用多播的缺点是需要通过网络发送更多的数据。
	// OnRep_则是“免费的”，因为旧值无需通过网络传递，而Credits本身就需要复制。
	// UFUNCTION(NetMulticast, Unreliable)
	// void MulticastCredits(float NewCredits, float Delta);		// 包含两个参数的RPC

public:

	UFUNCTION(BlueprintCallable, Category = "Credits")
	int32 GetCredits() const;
	
	UFUNCTION(BlueprintCallable, Category = "Credits")
	void AddCredits(int32 Delta);
	
	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool UseCredits(int32 Delta);

	UPROPERTY(BlueprintAssignable, Category = "Credits")
	FOnCreditsChanged OnCreditsChanged;

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);
};
