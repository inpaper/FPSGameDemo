// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"

#include "AIController.h"
#include "MyAIController.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAICharacter::AAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();

	float Temp = FMath::RandRange(0,1);
	if(Temp >= 0.5f)
	{
		AiTemp = EAITemp::Escape;
	}
	else
	{
		AiTemp = EAITemp::Angry;
	}
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAICharacter::Fire()
{
	Super::Fire();
}

void AAICharacter::AIFireBluePrint(AActor* FireToActor)
{
	FVector FireFromLocation = GunComponent->GetSocketLocation(FName("FirePoint"));
	FVector FireToLocation = FireToActor->GetActorLocation();
	
	FRotator FireRotator = UKismetMathLibrary::FindLookAtRotation(FireFromLocation,FireToLocation);
	FireComponent->Fire(ProjectileClass,FireFromLocation,FireRotator);

	Fire();
}

float AAICharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float TakeDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp,Warning,TEXT("AI受到伤害 %s %f"),*GetName(),DamageAmount);

	// 被玩家攻击到
	if(Cast<APlayerCharacter>(DamageCauser) != nullptr)
	{
		PatrolStatusBeAttacked(DamageCauser);
	}
	
	// 承受伤害者为AI时需要在空血时停止行动
	// 第一次死亡
	if(CurrentHP <= 0 && !bDead)
	{
		bDead = true;
		NotifyHPZero();
		ChangeAILive(false);
	}

	// 在血量低于40的时候激发AI的性格
	if(CurrentHP <= 40 && !bDead)
	{
		if(AiTemp == EAITemp::Escape)
		{
			UE_LOG(LogTemp,Warning,TEXT("Escape %s %f"),*GetName(),DamageAmount);
			AIEscape();
		}
		else if(AiTemp == EAITemp::Angry)
		{
			UE_LOG(LogTemp,Warning,TEXT("Angry %s %f"),*GetName(),DamageAmount);
			AIAngry();
		}
	}
	
	return TakeDamage;
}


void AAICharacter::ChangeAILive_Implementation(bool isAlive)
{
	bAIAlive = isAlive;
	NotifyToBehaviorTreeIsAlive(isAlive);
}
