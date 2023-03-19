// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"


void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(AttachedActor)) {
		RemoveFromParent();

		UE_LOG(LogTemp, Log, TEXT("AttachedActor no longer valid, removing Health Widget."));
		return;
	}

	FVector2D ScreenPosition;	// Êä³ö
	bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition);
	if (bIsOnScreen) {

		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition += ScreenOffset;
		ScreenPosition /= Scale;

		if (ParentSizeBox) {
			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}
	}

	if (ParentSizeBox) {
		ParentSizeBox->SetVisibility(bIsOnScreen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}