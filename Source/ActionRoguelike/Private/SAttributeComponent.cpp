// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"
#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("fm.DamageMultiplier"), 1.0f, TEXT("Global Damage Multiplier for Attribute Component."), ECVF_Cheat);

USAttributeComponent::USAttributeComponent()
{
	HealthMax = 100.0f;
	Health = HealthMax;

	RageMax = 100.0f;
	Rage = 0.0f;

	SetIsReplicatedByDefault(true);
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f) {
		return false;
	}

	if (Delta < 0.0f) {
		Delta *= CVarDamageMultiplier.GetValueOnGameThread();
	}

	float OldHealth = Health;
	float NewHealth = FMath::Clamp(Health + Delta, 0.0f, HealthMax);
	float ActualDelta = NewHealth - OldHealth;

	if (GetOwner()->HasAuthority()) {		// 只有服务器可以真正修改Health
		Health = NewHealth;
		if (ActualDelta != 0.0f) {
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
		}

		if (ActualDelta < 0.0f && Health == 0.0f) {
			ASGameModeBase* GM = Cast<ASGameModeBase>(GetWorld()->GetAuthGameMode());
			if (GM) {
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	return ActualDelta != 0.0f;
}

bool USAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	float OldRage = Rage;
	float NewRage = FMath::Clamp(Rage + Delta, 0.0f, RageMax);
	float ActualDelta = NewRage - OldRage;

	if (GetOwner()->HasAuthority()) {
		Rage = NewRage;
		if (ActualDelta != 0.0f) {
			MulticastRageChanged(InstigatorActor, Rage, ActualDelta);
		}
	}

	return ActualDelta != 0.0f;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return Health == HealthMax;
}

float USAttributeComponent::GetHealth() const
{
	return Health;
}

float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

float USAttributeComponent::GetRage() const
{
	return Rage;
}

float USAttributeComponent::GetRageMax() const
{
	return RageMax;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -HealthMax);

}

USAttributeComponent* USAttributeComponent::GetAttributeComp(AActor* FromActor)
{
	if (FromActor) {
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* AActor)
{
	USAttributeComponent* AttributeComp = GetAttributeComp(AActor);
	if (AttributeComp) {
		return AttributeComp->IsAlive();
	}

	return false;
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

void USAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewRage, Delta);
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);			// 无条件将服务器端的信息发送给所有客户端
	DOREPLIFETIME(USAttributeComponent, HealthMax);

	// DOREPLIFETIME_CONDITION(USAttributeComponent, HealthMax, COND_InitialOnly);

	DOREPLIFETIME(USAttributeComponent, Rage);
	DOREPLIFETIME(USAttributeComponent, RageMax);
}