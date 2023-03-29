// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SPowerupActor.generated.h"

class USphereComponent;

UCLASS(ABSTRACT)
class ACTIONROGUELIKE_API ASPowerupActor : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	ASPowerupActor();

protected:

	UPROPERTY(ReplicatedUsing = "OnRep_IsActive")
	bool bIsActive;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Powerup")
	float ReSpawnTime;

	FTimerHandle ReSpawnTimer;

	UFUNCTION()
	void OnRep_IsActive();

	UFUNCTION()
	void ShowPowerup();

	void HideAndCooldownPowerup();

	void SetPowerupState(bool bNewIsActive);

public:

	FText GetInteractText_Implementation(APawn* InstigatorPawn) override;

	void Interact_Implementation(APawn* InstigatorPawn) override;
};
