// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "BaseCharacter.generated.h"

// 基本的玩家类型

UCLASS()
class FPSGAMEDEMO_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ABaseCharacter();

	UPROPERTY(EditAnywhere,Category="Gun")
	USpotLightComponent* GunSpotLightComponent;
	
	UPROPERTY(EditAnywhere,Category="Camera")
	USpringArmComponent* CameraSpringArmComponent;

	// 内部实现的摄像机设置不了初始旋转度数 原因：spring组件使用了bUsePawnControlRotation后控制权不在spring组件上
	// TODO 开始时如何调整摄像机角度
	UPROPERTY(EditAnywhere,Category= "Camera")
	UCameraComponent* CameraComponent;
	
	UPROPERTY(EditAnywhere,Category="Gun")
	USkeletalMeshComponent* GunComponent;

	UPROPERTY(EditAnywhere,Category=Fire)
	UFireComponent* FireComponent;

	// 子弹生成物
	UPROPERTY(EditDefaultsOnly,Category=Fire)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere,Category=Camera)
	float StartCameraTurnPitch = 15.0f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 都有射击功能
	virtual void Fire();

	//----------------
	// TODO 都有伤害计算功能
	//---------------
};
