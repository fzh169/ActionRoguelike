// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

// 声明动态多播代理
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAttributeComponent();

protected:

	// EditAnywhere：可以在BP编辑器和关卡中的每个实例中进行编辑
	// VisibleAnywhere：在BP编辑器和关卡中只读访问（用于组件）
	// EditDefaultsOnly：隐藏每个实例的变量，仅在BP编辑器中编辑
	// VisibleDefaultsOnly：仅在BP编辑器中对变量进行只读访问（不常见）
	// EditInstanceOnly：仅允许编辑实例（例如：放置在关卡中时）
	// 
	// BlueprintReadOnly：蓝图脚本中只读访问（不影响细节面板）
	// BlueprintReadWrite：蓝图脚本中读写访问
	// 
	// Category = ""：仅在细节面板和蓝图上下文菜单中显示

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Health;

	// HealthMax, Stamina, Strength

public:	

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(float Delta);
};
