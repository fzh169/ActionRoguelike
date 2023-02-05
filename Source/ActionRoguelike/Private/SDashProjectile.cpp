// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ASDashProjectile::ASDashProjectile()
{
	TeleportDelay = 0.2f;
	DetonateDelay = 0.2f;

	MoveComp->InitialSpeed = 6000.0f;
}

void ASDashProjectile::Explode_Implementation()
{
	// 跳过基类实现，否则会提前摧毁
	// Super::Explode_Implementation();

	// 爆炸可能通过另一个源（如OnActorHit）调用，需要清除计时器防止调用两次
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

	FVector Scale(0.1f, 0.1f, 1.0f);
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation(), Scale);

	EffectComp->DeactivateSystem();		// 禁用粒子系统

	MoveComp->StopMovementImmediately();	// 停止移动
	SetActorEnableCollision(false);			// 关闭碰撞，防止再次触发Hit或重叠事件

	FTimerHandle TimerHandle_DelayedTeleport;
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &ASDashProjectile::TeleportInstigator, TeleportDelay);
}

void ASDashProjectile::TeleportInstigator()
{
	AActor* ActorToTeleport = GetInstigator();
	if (ensure(ActorToTeleport)) {
		// 保持Instigator的旋转，否则可能会引起震动
		ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation(), false, false);
	}

	Destroy();
}

void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &ASDashProjectile::Explode, DetonateDelay);
}
