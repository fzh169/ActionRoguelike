// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckHealth.h"
#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

USBTService_CheckHealth::USBTService_CheckHealth()
{
	LowHealthFraction = 0.3f;
}

void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ensure(AIPawn)) {

		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(AIPawn->GetComponentByClass(USAttributeComponent::StaticClass()));

		if (ensure(AttributeComp)) {

			bool bLowHealth = (AttributeComp->GetHealth() / AttributeComp->GetHealthMax()) < LowHealthFraction;

			UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
			BBComp->SetValueAsBool(LowHealthKey.SelectedKeyName, bLowHealth);
		}
	}
}
