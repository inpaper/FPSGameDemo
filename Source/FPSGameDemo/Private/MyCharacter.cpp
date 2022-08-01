// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameDemo/Public/MyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	GunComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Gun");
	GunSpotLightComponent = CreateDefaultSubobject<USpotLightComponent>("GunSpotLightComponent");
	
	// 锁定鼠标旋转，让鼠标旋转的时候不旋转人物
	// bUseControllerRotationPitch = false;
	// bUseControllerRotationYaw = false;
	// bUseControllerRotationRoll = false;

	// Configure character movement
	// GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	// GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	// GetCharacterMovement()->JumpZVelocity = 600.f;
	// GetCharacterMovement()->AirControl = 0.2f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	// CameraComponent->bUsePawnControlRotation = false;

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("CameraSpringArmComponent");
	// TODO 设置不了初始旋转度数
	const FRotator SpringRot = FRotator::FRotator(-20.0f,.0f,.0f);
	CameraSpringArmComponent->SetRelativeRotation(SpringRot);
	CameraSpringArmComponent->AttachTo(RootComponent);
	CameraSpringArmComponent->TargetArmLength = 600.0f;
	
	
	CameraSpringArmComponent->bUsePawnControlRotation = true;
	CameraComponent->AttachTo(CameraSpringArmComponent);
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(GunComponent == nullptr)return;
	GunComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GunPoint"));
	GunSpotLightComponent->AttachToComponent(GunComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("LightPoint"));
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward",this,&AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("MouseX", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("MouseY", this, &APawn::AddControllerPitchInput);

	// 实现了控制八方向移动情况。开发过程中先注释
	PlayerInputComponent->BindAction("OnlyXOrY",IE_Pressed,this,&AMyCharacter::StartRunEight);
	PlayerInputComponent->BindAction("OnlyXOrY",IE_Released,this,&AMyCharacter::StopRunEight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

// 更改当前是否可以八方向移动的情况
void AMyCharacter::StartRunEight()
{
	bRunEight = true;
}

void AMyCharacter::StopRunEight()
{
	bRunEight = false;
}

void AMyCharacter::MoveForward(float Value)
{
	if(GetController())
	{
		if(Value == 0.0f)
		{
			bRunY = false;
			return;
		}
		if(bRunX && !bRunEight)return;
		bRunY = true;
		
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if ( GetController())
	{
		if(Value == 0.0f)
		{
			bRunX = false;
			return;
		}

		if(bRunY && !bRunEight)return;
		bRunX = true;
		
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}



