// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "MyPlayerController.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArmComponent->bUsePawnControlRotation = true;

	FireBoomComponent = CreateDefaultSubobject<UFireBoomComponent>("FireBoomComponent");
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!IsLocallyControlled())return;

	// 由玩家控制
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController());
	if(PlayerController == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("PlayerController is nullptr"));
		return;
	}

	int SizeX,SizeY;
	PlayerController->GetViewportSize(SizeX,SizeY);
	
	float AimX,AimY;
	AimX = SizeX * AimPercentX;
	AimY = SizeY * AimPercentY;

	FVector WorldLocation,WorldDirection;
	PlayerController->DeprojectScreenPositionToWorld(AimX,AimY,WorldLocation,WorldDirection);
	
	float Ans = FVector::DotProduct(WorldDirection,GetActorUpVector());

	NotifyServerLookForward(Ans);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward",this,&APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("MouseX", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("MouseY", this, &APawn::AddControllerPitchInput);

	// 实现控制八方向移动情况。
	PlayerInputComponent->BindAction("PlusBtn",IE_Pressed,this,&APlayerCharacter::StartPlusMode);
	PlayerInputComponent->BindAction("PlusBtn",IE_Released,this,&APlayerCharacter::EndPlusMode);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// 电脑端使用
	PlayerInputComponent->BindAction("Fire",IE_Pressed,this,&APlayerCharacter::Fire);

	// 电脑端使用
	PlayerInputComponent->BindAction("FireBoom",IE_Pressed,this,&APlayerCharacter::FireBoomIndicatorOpen);
	PlayerInputComponent->BindAction("FireBoom",IE_Released,this,&APlayerCharacter::FireBoomIndicatorClose);
}

void APlayerCharacter::Fire()
{
	Super::Fire();
	
	FVector FireLocation = GunComponent->GetSocketLocation(FName("FirePoint"));

	if(!bFirePlus)
	{
		FRotator FireRotator;
		CalculateFireRotator(AimPercentX,AimPercentY,FireLocation,FireRotator);
		FireComponent->Fire(ProjectileClass,FireLocation,FireRotator);
	}
	// 四方位发射子弹
	else
	{
		FRotator FireRotator;
		
		CalculateFireRotator(0.5f,0.3f,FireLocation,FireRotator);
		FireComponent->Fire(ProjectileClass,FireLocation,FireRotator);

		CalculateFireRotator(0.5f,0.5f,FireLocation,FireRotator);
		FireComponent->Fire(ProjectileClass,FireLocation,FireRotator);

		CalculateFireRotator(0.4f,0.5f,FireLocation,FireRotator);
		FireComponent->Fire(ProjectileClass,FireLocation,FireRotator);

		CalculateFireRotator(0.6f,0.5f,FireLocation,FireRotator);
		FireComponent->Fire(ProjectileClass,FireLocation,FireRotator);
	}
}

void APlayerCharacter::CalculateFireRotator(const float PercentX,const float PercentY,const FVector FireLocation,FRotator& FireRotator)
{
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController());
	if(PlayerController == nullptr)return;

	int SizeX,SizeY;
	PlayerController->GetViewportSize(SizeX,SizeY);
	
	float AimX,AimY;
	AimX = SizeX * PercentX;
	AimY = SizeY * PercentY;

	FVector WorldLocation,WorldDirection;
	// 服务器上使用DeprojectScreenPositionToWorld会因为获取不到ULocalPlayer而输出位置为0
	// 因此DeprojectScreenPositionToWorld在客户端进行计算
	// 将屏幕位置转换为场景空间
	PlayerController->DeprojectScreenPositionToWorld(AimX,AimY,WorldLocation,WorldDirection);
	
	FVector HitLocation;
	HitLocation = WorldLocation + WorldDirection * FireLength;

	FireRotator = UKismetMathLibrary::FindLookAtRotation(FireLocation,HitLocation);
}


void APlayerCharacter::FireBoomIndicatorOpen()
{
	// 信息从客户端发送到服务端，然后变量自动复制
	FireBoomComponent->Init(GunComponent,ProjectileBoomClass,IndicatorBeamParticle);
	FireBoomComponent->IndicatorLineOpen();
}

void APlayerCharacter::FireBoomIndicatorClose()
{
	FireBoomComponent->IndicatorLineClose();
}

void APlayerCharacter::StartPlusMode()
{
	StartRunEight();
	bFirePlus = true;
}

void APlayerCharacter::EndPlusMode()
{
	StopRunEight();
	bFirePlus = false;
}


// 更改当前是否可以八方向移动的情况
void APlayerCharacter::StartRunEight()
{
	bRunEight = true;
}

void APlayerCharacter::StopRunEight()
{
	bRunEight = false;
}

void APlayerCharacter::MoveForward(float Value)
{
	YValue = Value;
	Move();
}

void APlayerCharacter::MoveRight(float Value)
{
	XValue = Value;
	Move();
}

void APlayerCharacter::Move()
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

void APlayerCharacter::NotifyServerLookForward_Implementation(float LookForward)
{
	if(!HasAuthority())return;
	
	GetLookForward = LookForward;
	NotifyClientsLookForward(LookForward);
}

void APlayerCharacter::NotifyClientsLookForward_Implementation(float LookForward)
{
	GetLookForward = LookForward;
}

void APlayerCharacter::GetMessageToRefreshHPName_Implementation(const FString& NewName)
{
	SendHPNameMessageToUMG(NewName);
}

