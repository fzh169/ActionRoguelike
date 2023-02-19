// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_HealSelf.h"
#include "AIController.h"
#include "SAttributeComponent.h"

EBTNodeResult::Type USBTTask_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (AIPawn == nullptr) {

		return EBTNodeResult::Failed;
	}

	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(AIPawn->GetComponentByClass(USAttributeComponent::StaticClass()));

	if (ensure(AttributeComp)) {

		AttributeComp->ApplyHealthChange(AIPawn, AttributeComp->GetHealthMax());
	}

	return EBTNodeResult::Succeeded;
}
