// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "AICharacter.h"
#include "MyPlayerState.h"
#include "PlayerCharacter.h"
#include "TargetScoreLogit.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 设置碰撞体
	CollisionMesh = CreateDefaultSubobject<USphereComponent>("CollisionMesh");
	CollisionMesh->InitSphereRadius(5.0f);
	SetRootComponent(CollisionMesh);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComponent->AttachToComponent(CollisionMesh,FAttachmentTransformRules::KeepRelativeTransform);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	// 让子弹创建好后自动发射出去
	ProjectileMovementComponent->UpdatedComponent = CollisionMesh;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;

	LaunchParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>("LaunchParticleComponent");
	LaunchParticleComponent->bAutoActivate = true;
	LaunchParticleComponent->AttachToComponent(MeshComponent,FAttachmentTransformRules::KeepRelativeTransform);
	
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionMesh->OnComponentHit.AddDynamic(this,&AProjectile::OnHit);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 碰撞检测
void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 不处理击中地板的子弹
	if(OtherActor->GetName() == TEXT("Floor"))
	{
		// 不做任何处理
	}
	// 只在服务器处理碰撞带来的事件，在其他仅实现物理效果
	else if(GetLocalRole() != ROLE_Authority)
	{
		// 子弹击中靶子或其他模拟物理物体
		if(OtherActor != nullptr && OtherActor != this && OtherComp != nullptr && OtherActor != SourcePlayer && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * HitForceBonus,GetActorLocation());
		}
	}
	// 子弹击中玩家或AI
	else if(OtherActor != nullptr && OtherActor != this && OtherComp != nullptr && OtherActor != SourcePlayer && Cast<ABaseCharacter>(OtherActor))
	{
		if(SourcePlayer == nullptr)
		{
			UE_LOG(LogTemp,Warning,TEXT("%s Projectile No SourcePlayer"),*GetName());
			Destroy();
			return;
		}
		
		UGameplayStatics::ApplyDamage(
				OtherActor,
				HitDamage,
				SourcePlayer->GetController(),
				SourcePlayer,
				UDamageType::StaticClass()
			);
	}
	// 子弹击中靶子或其他模拟物理物体
	else if(OtherActor != nullptr && OtherActor != this && OtherComp != nullptr && OtherActor != SourcePlayer && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * HitForceBonus,GetActorLocation());
		
		// 根据击中点距离靶心的距离判断是否正确击中靶子而并非靶架
		if(OtherComp->DoesSocketExist(FName("TargetCenter")))
		{
			FVector TargetCenter = OtherComp->GetSocketLocation(FName("TargetCenter"));
			FVector CurHitLoc = Hit.Location;
			float Distance = FVector::Distance(CurHitLoc,TargetCenter);

			if(Distance >= 100)
			{
				// 击中靶架
				UE_LOG(LogTemp,Warning,TEXT("击中靶架"));
			}
			else
			{
				// 击中靶心
				UE_LOG(LogTemp,Warning,TEXT("击中靶心"));
				if(SourcePlayer == nullptr)
				{
					UE_LOG(LogTemp,Warning,TEXT("%s Projectile No SourcePlayer"),*GetName());
					Destroy();
					return;
				}

				// 只有第一个击中靶心的才会加分
				UActorComponent* TargetComponent = OtherActor->GetComponentByClass(UTargetScoreLogit::StaticClass());
				if(TargetComponent == nullptr)
				{
					UE_LOG(LogTemp,Warning,TEXT("%s HitActor Miss UTargetScoreLogit"),*GetName());
					Destroy();
					return;
				}
				UTargetScoreLogit* TargetScoreLogitComponent = Cast<UTargetScoreLogit>(TargetComponent);
			
				if(TargetScoreLogitComponent->bHit)return;
				TargetScoreLogitComponent->bHit = true;

				UE_LOG(LogTemp,Warning,TEXT("%s"),*SourcePlayer->GetName());
				AMyPlayerState* DamageSourcePlayerState = Cast<AMyPlayerState>(SourcePlayer->GetPlayerState());
				// UE_LOG(LogTemp,Warning,TEXT("Score %d"),DamageSourcePlayerState->PlayerScore);
				DamageSourcePlayerState->AddScore(1);
			}
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("%s Actor %s Component Miss The Socket 'TargetCenter'"),*OtherActor->GetName(),*OtherComp->GetName());
		}
	}

	SetRootComponent(LaunchParticleComponent);
	CollisionMesh->SetNotifyRigidBodyCollision(false);
	CollisionMesh->DestroyComponent();
	MeshComponent->DestroyComponent();
}

