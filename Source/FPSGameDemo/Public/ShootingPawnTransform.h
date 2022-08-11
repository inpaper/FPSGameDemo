// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShootingPawnTransform.generated.h"

// 仅仅用于标定用户在传送到靶场后的位置定位Actor
UCLASS()
class FPSGAMEDEMO_API AShootingPawnTransform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShootingPawnTransform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
