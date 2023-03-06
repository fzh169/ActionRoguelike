// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"
#include "SWorldUserWidget.h"

static TAutoConsoleVariable<bool> CVarDrawDebugInteraction(TEXT("fm.InteractionDrawDebug"), false, TEXT("Enable Debug Lines and Spheres for Interact Component."), ECVF_Cheat);

USInteractionComponent::USInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceDistance = 500.0f;
	TraceRadius = 30.0f;
	CollisionChannel = ECC_WorldDynamic;
}

void USInteractionComponent::FindBestInteractable()
{
	bool bDrawDebug = CVarDrawDebugInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	// FHitResult Hit;
	// bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	TArray<FHitResult> Hits;

	FCollisionShape shape;
	shape.SetSphere(TraceRadius);

	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, shape);

	FocusedActor = nullptr;
	for (FHitResult Hit : Hits) {

		if (bDrawDebug) {
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, FColor::Blue, false, 2.0f);
		}

		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->Implements<USGameplayInterface>()) {
			FocusedActor = HitActor;
			break;
		}
	}

	if (FocusedActor) {
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass)) {
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}
		if (DefaultWidgetInstance) {
			DefaultWidgetInstance->AttachedActor = FocusedActor;
			if (!DefaultWidgetInstance->IsInViewport()) {
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else {
		if (DefaultWidgetInstance) {
			DefaultWidgetInstance->RemoveFromParent();
		}
	}

	if (bDrawDebug) {
		FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 2.0f);
	}
}

void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FindBestInteractable();
}

void USInteractionComponent::PrimaryInteract()
{
	if (FocusedActor == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No Focus Actor to interact.");
		return;
	}

	APawn* MyPawn = Cast<APawn>(GetOwner());
	ISGameplayInterface::Execute_Interact(FocusedActor, MyPawn);
}

