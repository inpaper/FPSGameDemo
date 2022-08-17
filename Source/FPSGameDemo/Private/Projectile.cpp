// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "AICharacter.h"
#include "MyPlayerState.h"
#include "PlayerCharacter.h"
#include "TargetScoreLogit.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 设置碰撞体
	CollisionMesh = CreateDefaultSubobject<USphereComponent>("CollisionMesh");
	CollisionMesh->InitSphereRadius(5.0f);
	SetRootComponent(CollisionMesh);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	// 让子弹创建好后自动发射出去
	ProjectileMovementComponent->UpdatedComponent = CollisionMesh;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	
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
	if(OtherActor->GetName() == TEXT("Floor"))return;

	// 子弹击中玩家或AI
	if(OtherActor != nullptr && OtherActor != this && OtherComp != nullptr && Cast<ABaseCharacter>(OtherActor))
	{
		// OtherComp->AddImpulseAtLocation(GetVelocity() * HitForceBonus,GetActorLocation());
	
		// 只在服务器处理碰撞带来的加分事件
		if(GetLocalRole() != ROLE_Authority)
		{
			UE_LOG(LogTemp,Warning,TEXT("Not Server"));
			return;
		}

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
		
		Destroy();
		return;
	}
	
	// 子弹击中靶子或其他模拟物理物体
	if(OtherActor != nullptr && OtherActor != this && OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * HitForceBonus,GetActorLocation());

		// 只在服务器处理碰撞带来的加分事件
		if(GetLocalRole() != ROLE_Authority)
		{
			UE_LOG(LogTemp,Warning,TEXT("Not Server"));
			return;
		}

		// 根据击中点距离靶心的距离判断是否正确击中靶子而并非靶架
		if(!OtherComp->DoesSocketExist(FName("TargetCenter")))
		{
			UE_LOG(LogTemp,Error,TEXT("%s Actor %s Component Miss The Socket 'TargetCenter'"),*OtherActor->GetName(),*OtherComp->GetName());
			Destroy();
			return;
		}
		
		FVector TargetCenter = OtherComp->GetSocketLocation(FName("TargetCenter"));
		FVector CurHitLoc = Hit.Location;
		float Distance = FVector::Distance(CurHitLoc,TargetCenter);

		if(Distance >= 100)
		{
			// 击中靶架
			UE_LOG(LogTemp,Warning,TEXT("击中靶架"));
		}else
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
			UE_LOG(LogTemp,Warning,TEXT("Score %d"),DamageSourcePlayerState->PlayerScore);
			DamageSourcePlayerState->AddScore(1);
		}
		
		Destroy();
		return;
	}
}

