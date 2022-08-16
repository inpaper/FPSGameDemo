// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

UCLASS()
class FPSGAMEDEMO_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable,Category="Fire")
	virtual void Fire() override;
public:
	// Sets default values for this character's properties
	AAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void AIFire();

	// 蓝图中使用行为树进行管理
	UFUNCTION(BlueprintCallable)
	void AIFireBluePrint(AActor* FireToActor);
	
};
