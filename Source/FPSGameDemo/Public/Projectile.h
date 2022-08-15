// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCharacter.h"
#include "PlayerCharacter.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

class USphereComponent;
UCLASS()
class FPSGAMEDEMO_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere,Category=Projectile)
	USphereComponent* CollisionMesh;
	
	UPROPERTY(VisibleAnywhere,Category=Projectile)
	UProjectileMovementComponent* ProjectileMovementComponent;

	// 保存投射出来的玩家，击中物体后给对应玩家加分
	APlayerCharacter* SourcePlayer;
	
	UPROPERTY(EditAnywhere,Category=Projectile)
	float HitForceBonus = 100.0f;

	// 碰撞事件
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );
};
