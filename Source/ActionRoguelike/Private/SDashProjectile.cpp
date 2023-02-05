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
	// ��������ʵ�֣��������ǰ�ݻ�
	// Super::Explode_Implementation();

	// ��ը����ͨ����һ��Դ����OnActorHit�����ã���Ҫ�����ʱ����ֹ��������
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

	FVector Scale(0.1f, 0.1f, 1.0f);
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation(), Scale);

	EffectComp->DeactivateSystem();		// ��������ϵͳ

	MoveComp->StopMovementImmediately();	// ֹͣ�ƶ�
	SetActorEnableCollision(false);			// �ر���ײ����ֹ�ٴδ���Hit���ص��¼�

	FTimerHandle TimerHandle_DelayedTeleport;
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &ASDashProjectile::TeleportInstigator, TeleportDelay);
}

void ASDashProjectile::TeleportInstigator()
{
	AActor* ActorToTeleport = GetInstigator();
	if (ensure(ActorToTeleport)) {
		// ����Instigator����ת��������ܻ�������
		ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation(), false, false);
	}

	Destroy();
}

void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &ASDashProjectile::Explode, DetonateDelay);
}
