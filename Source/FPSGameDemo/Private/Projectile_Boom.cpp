// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Boom.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

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
	
	BoomParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>("BoomParticleComponent");
	BoomParticleComponent->bAutoActivate = false;
	// TODO 虽然这里会Warning 但是目前只有依靠这个代码实现特效正确执行
	BoomParticleComponent->AttachToComponent(MeshComponent,FAttachmentTransformRules::KeepRelativeTransform);
	
	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>("RadialForceComponent");
	RadialForceComponent->Radius = 100.0f;
	RadialForceComponent->bAutoActivate = false;
	RadialForceComponent->AttachTo(RootComponent);
	
	InitialLifeSpan = ProjectileLife + ParticleDestroyTime;
}

// Called when the game starts or when spawned
void AProjectile_Boom::BeginPlay()
{
	Super::BeginPlay();

	CollisionMesh->OnComponentHit.AddDynamic(this,&AProjectile_Boom::OnHit);

	GetWorld()->GetTimerManager().SetTimer(BoomTimeHandle,this,&AProjectile_Boom::Boom,ProjectileLife,false);
}

// Called every frame
void AProjectile_Boom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 只有直接击中非地面外的物体会立刻爆炸，否则都是在一定时间后爆炸
void AProjectile_Boom::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	CollisionMesh->OnComponentHit.RemoveDynamic(this,&AProjectile_Boom::OnHit);
	
	if(OtherActor->GetName() == TEXT("Floor"))return;
	UE_LOG(LogTemp,Warning,TEXT("手榴弹击中物体 %s"),*OtherActor->GetName());

	Boom();
}

void AProjectile_Boom::Boom()
{
	GetWorld()->GetTimerManager().ClearTimer(BoomTimeHandle);
	
	BoomParticleComponent->Activate();

	SetRootComponent(BoomParticleComponent);
	CollisionMesh->SetNotifyRigidBodyCollision(false);
	CollisionMesh->DestroyComponent();
	MeshComponent->DestroyComponent();
	
	// BoomInServer();
	RadialForceComponent->FireImpulse();
	
	// 炸弹只对玩家血量进行伤害
	UGameplayStatics::ApplyRadialDamage(
		this,
		Damage,
		GetActorLocation(),
		RadialForceComponent->Radius,
		UDamageType::StaticClass(),
		TArray<AActor*>()
	);
}

void AProjectile_Boom::BoomInServer_Implementation()
{
	RadialForceComponent->FireImpulse();
}
