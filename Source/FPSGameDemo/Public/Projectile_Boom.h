// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
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
	
};
