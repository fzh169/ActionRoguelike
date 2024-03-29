// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"
#include "SAttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SActionComponent.h"
#include "../ActionRoguelike.h"

ASCharacter::ASCharacter()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;	// 摇臂使用pawn控制器的旋转值
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;	// 角色旋转朝向运动方向

	bUseControllerRotationYaw = false;	// 角色不使用控制器水平旋转，Pitch和Roll默认为false

	GetMesh()->SetGenerateOverlapEvents(true);

	TurnRate = 0.5f;
	LookUpRate = 0.5f;
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintStop);

	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::BlackHoleAttack);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);

	PlayerInputComponent->BindAction("Parry", IE_Pressed, this, &ASCharacter::Parry);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
}

void ASCharacter::HealSelf(float Amount /* = 50.0f */)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}

void ASCharacter::MoveForward(float value)
{
	FRotator ControlRot = GetControlRotation();		// 控制器当前旋转方向
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	
	AddMovementInput(ControlRot.Vector(), value);
}

void ASCharacter::MoveRight(float value)
{
	FRotator ControlRot = GetControlRotation();		// 控制器当前旋转方向
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	// X = Forward (Red)
	// Y = Right (Green)
	// Z = Up (Blue)

	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, value);
}

void ASCharacter::Turn(float value)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->IsLocalPlayerController() && PC->IsInputKeyDown(EKeys::RightMouseButton)) {
		AddControllerYawInput(TurnRate * value);
	}
}

void ASCharacter::LookUp(float value)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->IsLocalPlayerController() && PC->IsInputKeyDown(EKeys::RightMouseButton)) {
		AddControllerPitchInput(LookUpRate * value);
		FRotator ControlRot = GetControlRotation();
		// LogOnScreen(this, FString::Printf(TEXT("Running: %s"), *ControlRot.ToString()), FColor::Green);
		if (!((ControlRot.Pitch >= 0.0f && ControlRot.Pitch <= 10.0f) || (ControlRot.Pitch >= 330.0f && ControlRot.Pitch <= 360.0f))) {
			if (FMath::Abs(ControlRot.Pitch - 10.0f) < 10.0f)
				ControlRot.Pitch = 10.0f;
			else
				ControlRot.Pitch = 330.0f;
			PC->SetControlRotation(ControlRot);
		}
	}
}

void ASCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}

void ASCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}

void ASCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this, "Magic");
}

void ASCharacter::BlackHoleAttack()
{
	ActionComp->StartActionByName(this, "BlackHole");
}

void ASCharacter::Dash()
{
	ActionComp->StartActionByName(this, "Dash");
}

void ASCharacter::Parry()
{
	ActionComp->StartActionByName(this, "Parry");
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp) {
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f) {

		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		float RageDelta = FMath::Abs(Delta);
		AttributeComp->ApplyRageChange(InstigatorActor, RageDelta);
	}

	if (NewHealth <= 0.0f && Delta < 0.0f) {

		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
		SetLifeSpan(10.0f);
	}
}

FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}
