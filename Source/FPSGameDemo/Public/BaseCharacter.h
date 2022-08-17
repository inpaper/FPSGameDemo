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
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category= "Camera")
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

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=HP)
	int32 MaxHP = 100;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=HP)
	int32 CurrentHP = 100;
	
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

	// 只在服务器运行
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// 通知所有扣血的Pawn，客户端展示减少血量
	UFUNCTION(NetMulticast,Unreliable)
	void GetMessageToTakeDamage(int32 CurrentHPMessage);

	UFUNCTION(BlueprintImplementableEvent)
	void SendCurrentHPToUMG(int32 CurrentHPMessage);

	// 通知攻击者，在攻击者的UMG上刷新被攻击到的敌人的血量（方法在攻击者执行）
	UFUNCTION(Client,Unreliable)
	void GetMessageToShowHPUMG(ABaseCharacter* InjuredPawn);

	// 在攻击者的客户端上去执行被攻击者展示血量
	UFUNCTION(BlueprintImplementableEvent)
	void SendForceShowHPToUMG();
};
