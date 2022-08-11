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
	
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("CameraSpringArmComponent");
	
	// spring使用了pawn控制，因此设置不了初始旋转度数
	// const FRotator SpringRot = FRotator::FRotator(-20.0f,.0f,.0f);
	// CameraSpringArmComponent->SetRelativeRotation(SpringRot);
	
	CameraSpringArmComponent->AttachTo(RootComponent);
	CameraSpringArmComponent->TargetArmLength = 600.0f;
	
	CameraSpringArmComponent->bUsePawnControlRotation = true;
	CameraComponent->AttachTo(CameraSpringArmComponent);

	FireComponent = CreateDefaultSubobject<UFireComponent>("FireComponent");
	FireBoomComponent = CreateDefaultSubobject<UFireBoomComponent>("FireBoomComponent");
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
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

	// 实现控制八方向移动情况。
	PlayerInputComponent->BindAction("OnlyXOrY",IE_Pressed,this,&AMyCharacter::StartRunEight);
	PlayerInputComponent->BindAction("OnlyXOrY",IE_Released,this,&AMyCharacter::StopRunEight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// 电脑端使用
	PlayerInputComponent->BindAction("Fire",IE_Pressed,this,&AMyCharacter::FireBullet);

	// 电脑端使用
	PlayerInputComponent->BindAction("FireBoom",IE_Pressed,this,&AMyCharacter::FireBoomIndicatorOpen);
	PlayerInputComponent->BindAction("FireBoom",IE_Released,this,&AMyCharacter::FireBoomIndicatorClose);
}

void AMyCharacter::FireBullet()
{
	FVector FireLocation = GunComponent->GetSocketLocation(FName("FirePoint"));
	FireComponent->Fire(ProjectileClass,FireLocation,CameraComponent);
}

void AMyCharacter::FireBoomIndicatorOpen()
{
	// 信息从客户端发送到服务端，然后变量自动复制
	FireBoomComponent->Init(GunComponent,ProjectileBoomClass,IndicatorBeamParticle);
	FireBoomComponent->IndicatorLineOpen();
}

void AMyCharacter::FireBoomIndicatorClose()
{
	FireBoomComponent->IndicatorLineClose();
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
	YValue = Value;
	Move();
	// if(GetController())
	// {
	// 	if(Value == 0.0f)
	// 	{
	// 		bRunY = false;
	// 		return;
	// 	}
	//
	// 	// 限制八方向位移
	// 	if(bRunX && !bRunEight)return;
	// 	bRunY = true;
	// 	
	// 	const FRotator Rotation = GetController()->GetControlRotation();
	// 	const FRotator YawRotation(0, Rotation.Yaw, 0);
	// 	
	// 	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// 	AddMovementInput(Direction, Value);
	// }
}

void AMyCharacter::MoveRight(float Value)
{
	XValue = Value;
	Move();
	// if ( GetController())
	// {
	// 	if(Value == 0.0f)
	// 	{
	// 		bRunX = false;
	// 		return;
	// 	}
	//
	// 	// 限制八方向位移
	// 	if(bRunY && !bRunEight)return;
	// 	bRunX = true;
	// 	
	// 	const FRotator Rotation = GetController()->GetControlRotation();
	// 	const FRotator YawRotation(0, Rotation.Yaw, 0);
	// 	
	// 	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	// 	AddMovementInput(Direction, Value);
	// }
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp,Warning,TEXT("受到伤害 %s"),*GetActorLocation().ToString());
	
	int32 GetDamage = FPlatformMath::RoundToInt(DamageAmount);
	GetDamage = FMath::Clamp(GetDamage,0,CurrentHP);

	CurrentHP -= GetDamage;

	if(CurrentHP <= 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("%s 玩家血量归零"),*GetName());
	}

	return GetDamage;
}

void AMyCharacter::Move()
{
	if (!GetController())return;
	if(XValue == 0.0f && YValue == 0.0f)return;
	
	bool isX = FMath::Abs(XValue) > FMath::Abs(YValue);
	
	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	if(isX || bRunEight)
	{
		const FVector DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(DirectionX, XValue);
	}
	if(!isX || bRunEight)
	{
		const FVector DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(DirectionY, YValue);
	}
}




