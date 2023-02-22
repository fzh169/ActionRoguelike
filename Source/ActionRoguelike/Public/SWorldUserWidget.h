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

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	AActor* AttachedActor;	// ����ϵͳ���ݻ��Զ��ÿ�ָ��

protected:

	UPROPERTY(meta = (BindWidget))
	USizeBox* ParentSizeBox;

	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldOffset;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};
