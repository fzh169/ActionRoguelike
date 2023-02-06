// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

// ������̬�ಥ����
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAttributeComponent();

protected:

	// EditAnywhere��������BP�༭���͹ؿ��е�ÿ��ʵ���н��б༭
	// VisibleAnywhere����BP�༭���͹ؿ���ֻ�����ʣ����������
	// EditDefaultsOnly������ÿ��ʵ���ı���������BP�༭���б༭
	// VisibleDefaultsOnly������BP�༭���жԱ�������ֻ�����ʣ���������
	// EditInstanceOnly��������༭ʵ�������磺�����ڹؿ���ʱ��
	// 
	// BlueprintReadOnly����ͼ�ű���ֻ�����ʣ���Ӱ��ϸ����壩
	// BlueprintReadWrite����ͼ�ű��ж�д����
	// 
	// Category = ""������ϸ��������ͼ�����Ĳ˵�����ʾ

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Health;

	// HealthMax, Stamina, Strength

public:	

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(float Delta);
};
