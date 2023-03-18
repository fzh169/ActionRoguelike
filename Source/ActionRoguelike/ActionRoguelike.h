// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_STATS_GROUP(TEXT("FANMI_Game"), STATGROUP_FANMI, STATCAT_Advanced)

static void LogOnScreen(UObject* WorldContext, FString Msg, FColor Color = FColor::White, float Duration = 2.0f)
{
	if (!ensure(WorldContext)) {
		return;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!ensure(World)) {
		return;
	}
	
	FString NetPrefix = "";
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, NetPrefix + Msg);
	}
}