// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "SDashProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASDashProjectile : public ASProjectileBase
{
	GENERATED_BODY()
	
public:

	ASDashProjectile();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float TeleportDelay;
	
	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float DetonateDelay;

	// 如果命中某物，则取消计时器的句柄
	FTimerHandle TimerHandle_DelayedDetonate;

	// 基类使用BlueprintNativeEvent，必须重写_Implementation而非Explode()
	virtual void Explode_Implementation() override;

	void TeleportInstigator();

	virtual void BeginPlay() override;
};
