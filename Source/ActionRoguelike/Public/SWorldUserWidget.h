// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"

class USizeBox;

/**
 *
 */
UCLASS()
class ACTIONROGUELIKE_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (ExposeOnSpawn = true))
	AActor* AttachedActor;	// 属性系统：摧毁自动置空指针

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FVector WorldOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FVector2D ScreenOffset;

protected:

	UPROPERTY(meta = (BindWidget))
	USizeBox* ParentSizeBox;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
