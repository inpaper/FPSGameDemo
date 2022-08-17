// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Projectile_Boom.generated.h"

UCLASS()
class FPSGAMEDEMO_API AProjectile_Boom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile_Boom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere,Category=Projectile)
	USphereComponent* CollisionMesh;

	UPROPERTY(EditAnywhere,Category=Projectile)
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleAnywhere,Category=Projectile)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere,Category=Projectile)
	UParticleSystemComponent* BoomParticleComponent;
	
	UPROPERTY(EditAnywhere,Category=Projectile)
	URadialForceComponent* RadialForceComponent;

	// 只在服务器运行
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

	// 手榴弹爆炸最大时间
	UPROPERTY(EditAnywhere,Category=LifeTime)
	float ProjectileLife = 3.0f;
	
	// 特效删除时间
	UPROPERTY(EditAnywhere,Category=LifeTime)
	float ParticleDestroyTime = 2.0f;

	UPROPERTY(EditAnywhere,Category=Projectile)
	float Damage = 30.0f;
	
private:
	FTimerHandle BoomTimeHandle;

	// 炸弹表现
	UFUNCTION(NetMulticast,Reliable)
	void Boom();
};
