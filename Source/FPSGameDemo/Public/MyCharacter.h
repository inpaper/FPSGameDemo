// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	USkeletalMeshComponent* GunComponent;

	UPROPERTY(EditAnywhere,Category="Gun")
	USpotLightComponent* GunSpotLightComponent;
	
	UPROPERTY(EditAnywhere,Category= "Camera")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere,Category="Camera")
	USpringArmComponent* CameraSpringArmComponent;
public:
	// Sets default values for this character's properties
	AMyCharacter();

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
