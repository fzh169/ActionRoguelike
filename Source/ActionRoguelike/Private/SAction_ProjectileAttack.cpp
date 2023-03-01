// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_ProjectileAttack.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	AttackAnimDelay = 0.3f;
	HandSocketName = "Muzzle_01";
}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	// check verify ensure
	if (ensure(ProjectileClass)) {

		FVector ProjLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);		// FNameͨ��Hash���п��ٲ�ѯ

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;

		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);

		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();

		// �յ����۲�����Զ����̫Զ����δ����ʱ�Գ�ʮ���߷��������
		FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * 5000);

		FHitResult Hit;
		if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjParams, Shape, Params)) {

			TraceEnd = Hit.ImpactPoint;
		}

		// ��Muzzle_01ָ��ײ������·���/��ת
		FRotator ProjRotation = FRotationMatrix::MakeFromX(TraceEnd - ProjLocation).Rotator();

		FTransform SpawnTM = FTransform(ProjRotation, ProjLocation);	// ������λ�ã�

		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}

	StopAction(InstigatorCharacter);
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);

	if (Character) {
		Character->PlayAnimMontage(AttackAnim);
		UGameplayStatics::SpawnEmitterAttached(CastingEffect, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		FTimerHandle TimerHandle_AttackDelay;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
	}

}