// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<bool> CVarDrawDebugInteraction(TEXT("fm.InteractionDrawDebug"), false, TEXT("Enable Debug Lines and Spheres for Interact Component."), ECVF_Cheat);

USInteractionComponent::USInteractionComponent()
{
	//
}

void USInteractionComponent::PrimaryInteract()
{
	bool bDrawDebug = CVarDrawDebugInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + (EyeRotation.Vector() * 1000);

	// FHitResult Hit;
	// bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	TArray<FHitResult> Hits;

	float Radius = 30.0f;

	FCollisionShape shape;
	shape.SetSphere(Radius);

	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, shape);

	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	for (FHitResult Hit : Hits) {

		AActor* HitActor = Hit.GetActor();

		if (HitActor && HitActor->Implements<USGameplayInterface>()) {

			APawn* MyPawn = Cast<APawn>(MyOwner);

			ISGameplayInterface::Execute_Interact(HitActor, MyPawn);

			if (bDrawDebug) {

				DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColor, false, 2.0f);
			}

			break;

		}
	}

	if (bDrawDebug) {

		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 2.0f);
	}
}

