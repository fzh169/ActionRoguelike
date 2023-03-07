// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_HealthPotion.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SAttributeComponent.h"
#include "SPlayerState.h"

ASPowerup_HealthPotion::ASPowerup_HealthPotion()
{
	HealAmount = 50.0f;
	CreditCost = 50;
}

void ASPowerup_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!(ensure(InstigatorPawn))) {
		return;
	}

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributeComp(InstigatorPawn);
	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth()) {

		ASPlayerState* PS = InstigatorPawn->GetPlayerState<ASPlayerState>();
		if (PS && PS->UseCredits(CreditCost) && AttributeComp->ApplyHealthChange(this, HealAmount)) {
			HideAndCooldownPowerup();
		}

	}
}
