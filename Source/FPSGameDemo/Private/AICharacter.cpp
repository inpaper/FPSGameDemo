// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"

#include "AIController.h"
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

