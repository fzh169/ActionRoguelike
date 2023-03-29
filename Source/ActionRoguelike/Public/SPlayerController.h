// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	UUserWidget* PauseMenuInstance;
	
	UPROPERTY(BlueprintAssignable)
	FOnPawnChanged OnPawnChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChanged OnPlayerStateReceived;

	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

	void SetupInputComponent() override;

	virtual void SetPawn(APawn* InPawn) override;

	// 当Player Controller准备好开始时调用，用于初始化UI之类的东西（在BeginPlay中可能过早）。
	// 尤其是在多人游戏中，客户端可能尚未接收到所有数据，如PlayerState。
	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

	void OnRep_PlayerState() override;
};
