// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "SPowerup_HealthPotion.generated.h"

class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPowerup_HealthPotion : public ASPowerupActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "HealthPotion")
	float HealAmount;

	UPROPERTY(EditDefaultsOnly, Category = "HealthPotion")
	int32 CreditCost;

public:

	ASPowerup_HealthPotion();

	FText GetInteractText_Implementation(APawn* InstigatorPawn) override;

	void Interact_Implementation(APawn* InstigatorPawn) override;
};
