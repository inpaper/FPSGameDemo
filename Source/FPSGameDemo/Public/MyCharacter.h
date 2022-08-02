// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyCharacter.generated.h"

UCLASS()
class FPSGAMEDEMO_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,Category="Gun")
	USpotLightComponent* GunSpotLightComponent;
	
	UPROPERTY(EditAnywhere,Category="Camera")
	USpringArmComponent* CameraSpringArmComponent;
public:
	// Sets default values for this character's properties
	AMyCharacter();

	// 内部实现的摄像机设置不了初始旋转度数 原因：spring组件使用了bUsePawnControlRotation后控制权不在spring组件上
	// TODO 开始时如何调整摄像机角度
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category= "Camera")
	UCameraComponent* CameraComponent;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Gun")
	USkeletalMeshComponent* GunComponent;

	// 将该定义写在ActorComponent子类上，每次打开都需要重新设置，因此将该定义写入Character类中
	UPROPERTY(EditDefaultsOnly,Category=Projectile)
	TSubclassOf<class AProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere,Category="Camera")
	float StartCameraTurnPitch = 15.0f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	// 更改当前是否可以八方向移动的情况。
	void StartRunEight();
	void StopRunEight();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	// 控制八方向移动，限制只有X和Y不能同时输入
	bool bRunEight = false;

	UBoolProperty* BRunEight;

	// 判断是否当前正在按下X或者Y
	bool bRunX = false;
	bool bRunY = false;
};
