// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SAttributeComponent.h"

ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	TimeToHitParamName = "TimeToHit";
	TargetActorKey = "TargetActor";
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	AAIController* AIC = Cast<AAIController>(GetController());

	if (AIC) {
		UBlackboardComponent* BBComp = AIC->GetBlackboardComponent();
		BBComp->SetValueAsObject("TargetActor", Pawn);

		// DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
	}
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActorm, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{

}
