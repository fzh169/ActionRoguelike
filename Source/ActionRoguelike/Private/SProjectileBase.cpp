// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileBase.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASProjectileBase::ASProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	// SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
	// SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	// SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetCollisionProfileName("Projectile");
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMoveComp");
	MoveComp->bRotationFollowsVelocity = true;
	MoveComp->bInitialVelocityInLocalSpace = true;
	MoveComp->ProjectileGravityScale = 0.0f;
	MoveComp->InitialSpeed = 1000.0f;
}

void ASProjectileBase::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

// _Implementation����ִ�б��ΪBlueprintNativeEvent
void ASProjectileBase::Explode_Implementation()
{
	// ���ensure�����ȷ��û�б����ݻ١�
	if (ensure(!IsPendingKill())) {

		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

		Destroy();
	}
}

void ASProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
	
	// �������Ӵ˰�֮ǰ����ʹ��������ʱ��������Blueprint
	// ���빹�캯����ȣ��˴��󶨸�Ϊһ�£����캯�������޷���
	// PostInitializeComponent�ǰ��κ��¼�����ѡ��ʽ
	SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);
}
