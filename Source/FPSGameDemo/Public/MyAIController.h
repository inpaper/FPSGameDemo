// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

/**
 * 只在Server中存在，包括Tick也是在Server中运行
 */
UCLASS()
class FPSGAMEDEMO_API AMyAIController : public AAIController
{
	GENERATED_BODY()

	// TODO Tick没有执行，不清楚原因，使用GameMode的Tick来代替
	virtual void Tick(float DeltaSeconds) override;
	
public:
	void GetPlayerPawnTransform();
	
	UPROPERTY(BlueprintReadOnly)
	FVector AimToDirection;
	
	// AI转身速度，默认每秒钟可以转120度
	UPROPERTY(EditDefaultsOnly)
	float MaxDegreePerSecond = 120.0f;

	UFUNCTION(NetMulticast,Reliable)
	void NotifyClientsLookForward(float LookForward);
	
	// UFUNCTION(BlueprintImplementableEvent)
	// void NotifyClientsLookForwardToUMG(float LookForward);

	// UPROPERTY(Replicated)
	// float GetLookForward;
};
