// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAMEDEMO_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// 执行拥有该控制器的客户端需要执行的方法，如UI显示等
	UFUNCTION(Client,Reliable)
	void OwnerClientPostLogin();

	UUserWidget* HUD;

	TSubclassOf<UUserWidget> HUDWidget;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateHUD();

	UFUNCTION(Server,Reliable)
	void RespawnPlayerPawn();

	// 更新积分榜中玩家信息
	UFUNCTION(Client,Unreliable)
	void RefreshPlayerInfo(const TArray<FString>& PlayerName,const TArray<int32>& PlayerScore);
	
	UFUNCTION(BlueprintImplementableEvent)
	void FillHUDPlayerInfo(const TArray<FString>& PlayerName,const TArray<int32>& PlayerScore);

	// 更新积分榜中玩家信息
	UFUNCTION(Client,Unreliable)
	void RefreshPlayerHP(const TArray<FString>& PlayerName,const TArray<int32>& PlayerHP);
	
	UFUNCTION(BlueprintImplementableEvent)
	void FillHUDPlayerHP(const TArray<FString>& PlayerName,const TArray<int32>& PlayerHP);
	
	// 通知游戏中玩家数量
	UFUNCTION(Client,Reliable)
	void NotifyPlayerSum(int PlayerSum);
	
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshPlayerSum(int PlayerSum);

	//---------------------------------------------------------------------------------
	// 基础流程：玩家点击按钮等待数秒后传送，传送到靶场后等待数秒后正式游戏，一定时间后游戏结束展示排行榜
	// TODO 后续考虑将以下逻辑写入GameMode中，目前放在这里和Controller的功能对应不上

	// 目前有两种游戏模式，输入0为靶场模式，输入1为AI场模式
	// 服务器中开始游戏，玩家点击开始游戏触发，
	// TODO 目前设置为任何一个人点击就可以开始游戏，后续可以做成全部客户端同意方可进入。
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void AskToStartGame(int32 GameType);

	// 服务器中将玩家传送到靶场
	UFUNCTION(Server,Reliable)
	void Pass(int32 GameType);

	UFUNCTION(Server,Reliable)
	void StartGame(int32 GameType);

	UFUNCTION(Server,Reliable)
	void EndGame();

	// 计算分数，展示排行榜
	UFUNCTION(Server,Reliable)
	void CalculateScore();

	// 游戏结束后回到等待区域
	UFUNCTION(BlueprintCallable,Server,Unreliable)
	void AskToBackWait();
	
	//---------------------------------
	// 以下脚本两两成对，客户端获取信息后去蓝图执行UI相关交互逻辑。
	// 使用Client进行封装，避开蓝图实现UI但不能同时使用RPC的问题
	
	// 获取到服务器发来的开始游戏信息，在WaitTime后Server进行传送
	UFUNCTION(Client,Reliable)
	void GetMessageToPass(int32 WaitTime,int32 GameType);

	UFUNCTION(BlueprintImplementableEvent)
	void SendReadyPassMessageToUMG(int32 WaitTime,int32 GameType);

	// 获取到服务器发来的等待游戏开始信息，在WaitTime后正式开始游戏
	UFUNCTION(Client,Reliable)
	void GetMessageToReadyGame(int32 WaitTime);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SendReadyGameMessageToUMG(int32 WaitTime);

	// 获取到服务器发来的等待游戏开始信息，在WaitTime后正式开始游戏
	UFUNCTION(Client,Reliable)
	void GetMessageToStartGame(int32 TotalGameTime);

	UFUNCTION(BlueprintImplementableEvent)
	void SendStartGameMessageToUMG(int32 TotalGameTime);

	// 获取到服务器发来的游戏结束及所有玩家名称和得分信息
	UFUNCTION(Client,Unreliable)
	void GetMessageToCalScore(const TArray<FString>& PlayerNumber,const TArray<int32>& PlayerScore);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SendCalScoreMessageToUMG(const TArray<FString>& PlayerNumber,const TArray<int32>& PlayerScore);

	// 获取到服务器发来的开始AI场游戏信息，隐藏HUD的部分UI
	UFUNCTION(Client,Reliable)
	void GetMessageToHideHUDUI(int32 GameType);

	UFUNCTION(BlueprintImplementableEvent)
	void SendHideHUDUIMessageToUMG(int32 GameType);

	UFUNCTION(Client,Unreliable)
	void GetMessageToBackWait();

	UFUNCTION(BlueprintImplementableEvent)
	void SendBackWaitMessageToUMG();
	
	//-----------------------
	// 生成AI的Pawn
	UFUNCTION(BlueprintCallable,Server,Unreliable)
	void AskToSpawnAIPawn();
	
private:
	// 用于服务器倒计时进入下一阶段，
	// TODO 鲁棒性不足，如果时间到了，游戏状态不对应，如何重新根据游戏状态表现游戏
	FTimerHandle PassTimeHandle;

	// 用于定时器传值使用，委托
	FTimerDelegate PassTimeDelegate;
};
