// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	GunComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Gun");
	GunSpotLightComponent = CreateDefaultSubobject<USpotLightComponent>("GunSpotLightComponent");
	
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("CameraSpringArmComponent");
	
	// spring使用了pawn控制，因此设置不了初始旋转度数
	// const FRotator SpringRot = FRotator::FRotator(-20.0f,.0f,.0f);
	// CameraSpringArmComponent->SetRelativeRotation(SpringRot);
	
	CameraSpringArmComponent->AttachTo(RootComponent);
	CameraSpringArmComponent->TargetArmLength = 600.0f;
	
	CameraComponent->AttachTo(CameraSpringArmComponent);

	FireComponent = CreateDefaultSubobject<UFireComponent>("FireComponent");
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(CameraSpringArmComponent != nullptr)
	{
		AddControllerPitchInput(StartCameraTurnPitch);
	}
	// 使枪的模型贴合人物手部骨骼点，并使SpotLight贴合到枪的射击口
	if(GunComponent != nullptr)
	{
		GunComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GunPoint"));
		if(GunSpotLightComponent)
		{
			GunSpotLightComponent->AttachToComponent(GunComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("LightPoint"));
		}
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::Fire()
{
	UE_LOG(LogTemp,Warning,TEXT("%s Fire"),*GetName());
}


