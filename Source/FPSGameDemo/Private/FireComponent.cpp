// Fill out your copyright notice in the Description page of Project Settings.


#include "FireComponent.h"

#include "PlayerCharacter.h"
#include "Projectile.h"
#include "Camera/CameraComponent.h"
#include "FPSGameDemo/FPSGameDemoGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UFireComponent::UFireComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFireComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	
	// 设置开枪的状态
	if(FPlatformTime::Seconds() - RecordTime < ReloadTime)
	{
		FireState = EFireState::Reload;
	}
	else
	{
		FireState = EFireState::Ready;
	}
}

// 暂时不适用该方法，直接在MyCharacter中设置投掷物类型，看后续蓝图是否会更新掉
// void UFireComponent::Init(TSubclassOf<AProjectile> ProjectileToSet)
// {
// 	ProjectileClass = ProjectileToSet;
// }

// 按下开火键执行该操作
void UFireComponent::Fire_Implementation(TSubclassOf<class AProjectile> GetProject,FVector FireLocation,FRotator FireDirection)
{
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(UGameplayStatics::GetGameMode(this));
	if(GameMode == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("GameMode Error"));
		return;
	}
	if(!GameMode->bFireAbility)
	{
		UE_LOG(LogTemp,Warning,TEXT("Controller Can not Fire"));
		return;
	}
	
	if(FireState == EFireState::Reload)return;
	
	AProjectile* CreateProjectile = GetWorld()->SpawnActor<AProjectile>(
		GetProject,
		FireLocation,
		FireDirection
	);
	RecordTime = FPlatformTime::Seconds();
	if(CreateProjectile == nullptr)return;
	CreateProjectile->SourcePlayer = Cast<ABaseCharacter>(GetOwner());
}

// Deprecated 弃用该方法，转而使用屏幕位置生成发射点位
// 通过计算摄像头的位置和摄像头正方向确定开始与结束位置，生成射线，将射线击中的点作为射击的点，从子弹初始位置看向击中位置获取发射方向。
void UFireComponent::GetFireDirection(FVector FireLocation,UCameraComponent* CameraComponent,FRotator &FireDirection)
{
	FVector StartLocation = CameraComponent->GetComponentLocation();
	FVector EndLocation = StartLocation + CameraComponent->GetForwardVector() * LineLength;
	
	FVector HitLocation = EndLocation;
	
	FHitResult HitResult;
	if(GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation,EndLocation,ECollisionChannel::ECC_Visibility))
	{
		HitLocation = HitResult.Location;
	}

	// 发射位置默认为枪位置，计算中使用摄像机位置，所以需要在结果处补充这个偏差。
	FVector GunCameraOffset = StartLocation - FireLocation;

	FireDirection = UKismetMathLibrary::FindLookAtRotation(StartLocation,HitLocation + GunCameraOffset);
}

