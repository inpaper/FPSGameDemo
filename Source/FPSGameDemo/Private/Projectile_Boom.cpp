// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Boom.h"

// Sets default values
AProjectile_Boom::AProjectile_Boom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 设置碰撞体
	CollisionMesh = CreateDefaultSubobject<USphereComponent>("CollisionMesh");
	CollisionMesh->InitSphereRadius(5.0f);
	SetRootComponent(CollisionMesh);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComponent->AttachTo(CollisionMesh);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	// 让子弹创建好后不会自动发射出去
	ProjectileMovementComponent->InitialSpeed = 0.0f;
	ProjectileMovementComponent->MaxSpeed = 0.0f;
	// ProjectileMovementComponent->UpdatedComponent = CollisionMesh;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.2;
	ProjectileMovementComponent->Velocity = FVector::ZeroVector;
	
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AProjectile_Boom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile_Boom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

