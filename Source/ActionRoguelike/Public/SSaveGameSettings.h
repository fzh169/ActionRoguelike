// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SSaveGameSettings.generated.h"

class UDataTable;

/**
 * 
 */
// defaultconfig：只将对象配置保存到默认INI，从不保存到本地INI。
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Save Game Settings"))
class ACTIONROGUELIKE_API USSaveGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	USSaveGameSettings();

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FString SaveSlotName;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General", AdvancedDisplay)
	TSoftObjectPtr<UDataTable> DummyTablePath;
};