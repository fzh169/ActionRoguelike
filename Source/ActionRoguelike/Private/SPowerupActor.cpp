// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

ASPowerupActor::ASPowerupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("ShpereComp");
	SphereComp->SetCollisionProfileName("Powerup");
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);		// 禁用碰撞，使用SphereComp处理交互查询
	MeshComp->SetupAttachment(SphereComp);

	ReSpawnTime = 10.0f;
	bReplicates = true;
	bIsActive = true;
}

void ASPowerupActor::OnRep_IsActive()
{
	SetActorEnableCollision(bIsActive);
	RootComponent->SetVisibility(bIsActive, true);	// 设置root和所有子组件的可见性
}

void ASPowerupActor::ShowPowerup()
{
	SetPowerupState(true);
}

void ASPowerupActor::HideAndCooldownPowerup()
{
	SetPowerupState(false);
	GetWorldTimerManager().SetTimer(ReSpawnTimer, this, &ASPowerupActor::ShowPowerup, ReSpawnTime);
}

void ASPowerupActor::SetPowerupState(bool bNewIsActive)
{
	bIsActive = bNewIsActive;		// 改变值触发客户端OnRep_IsActive()
	OnRep_IsActive();				// 服务器直接调用
}

FText ASPowerupActor::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::GetEmpty();
}

void ASPowerupActor::Interact_Implementation(APawn* InstigatorPawn)
{
	// derived classes
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bIsActive);
}
