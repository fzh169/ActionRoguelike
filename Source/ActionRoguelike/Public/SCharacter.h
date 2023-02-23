// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class USInteractionComponent;
class USAttributeComponent;

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> MagicProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> BlackHoleProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> DashProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* PrimaryAnim;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* BlackHoleAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* DashAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UParticleSystem* CastingEffect;

	float AttackAnimDelay;

	FTimerHandle TimerHandle_PrimaryAttack;
	FTimerHandle TimerHandle_BlackHoleAttack;
	FTimerHandle TimerHandle_Dash;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComp;

	virtual void PostInitializeComponents() override;

	void MoveForward(float value);
	void MoveRight(float value);

	void PrimaryAttack();
	void PrimaryAttack_TimeElapsed();

	void BlackHoleAttack();
	void BlackHoleAttack_TimeElapsed();

	void Dash();
	void Dash_Elapsed();

	void SpawnProjectile(TSubclassOf<AActor> ClassToSpawn, FVector ProjLocation);

	void PrimaryInteract();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Exec)		// 控制台函数，仅在Player Controller, Player Character, Game Mode, Cheat Manager 等类中有效
	void  HealSelf(float Amount = 100.0f);
};
