// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

// 默认服务器上的GameInstance的状态才是真实状态，
// TODO 客户端状态仅用于跟服务器状态核对？目前暂时没有使用
UENUM()
enum class EPlayerGameMode:uint8
{
	// 主菜单界面
	MainMenu,
	// 等待玩家进入
	WaitingPlayer,
	// 传送倒计时中，切换为靶场准备中即为传送进行的时刻
	WaitingPassing,
	// 靶场准备中
	PlayingReady,
	// 在靶场中
	PlayingTarget,
	// 在AI场中
	PlayingAI,
	
	Unknown,
};


/**
 * 
 */
UCLASS()
class FPSGAMEDEMO_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly,Category=GameState)
	EPlayerGameMode CurrentState = EPlayerGameMode::MainMenu;

	UFUNCTION(BlueprintCallable,Category=GameState)
	bool TransitionToState(EPlayerGameMode GetState);

	UFUNCTION(BlueprintCallable,Category=GameState)
	bool IsCurrentState(EPlayerGameMode GetState);
};
