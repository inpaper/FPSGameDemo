// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyCharacter.h"
#include "MyGameInstance.h"
#include "MyPlayerState.h"
#include "PlayerCharacter.h"
#include "FPSGameDemo/FPSGameDemoGameModeBase.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"

void AMyPlayerController::OwnerClientPostLogin_Implementation()
{
	// UE_LOG(LogTemp,Warning,TEXT("is client %s"),IsLocalController() ? TEXT("True") : TEXT("True"));
	// UE_LOG(LogTemp,Warning,TEXT("ROLE_Authority %s"),GetLocalRole() == ROLE_Authority ? TEXT("True") : TEXT("True"));
	UE_LOG(LogTemp,Warning,TEXT("服务器 %s"),HasAuthority() ? TEXT("True") : TEXT("True"));

	// 蓝图中处理UMG事件
	CreateHUD();

	// 服务器生成控制物体
	RespawnPlayerPawn();
}

void AMyPlayerController::RespawnPlayerPawn_Implementation()
{
	APawn* ControlledPawn = GetPawn();
	if(ControlledPawn != nullptr)
	{
		ControlledPawn->Destroy();
	}
	
	AGameModeBase* GetMode = UGameplayStatics::GetGameMode(this);
	UE_LOG(LogTemp,Warning,TEXT("服务器 %s"),HasAuthority() ? TEXT("True") : TEXT("True"));
	UE_LOG(LogTemp,Warning,TEXT("ROLE_Authority %s"),GetLocalRole() == ROLE_Authority ? TEXT("True") : TEXT("True"));

	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GetMode);

	AMyPlayerState* MyPlayerState = GetPlayerState<AMyPlayerState>();

	UE_LOG(LogTemp,Warning,TEXT("PlayerState->PlayerNumber %d"),MyPlayerState->PlayerNumber);
	
	AActor* GameStart = GameMode->GetGameStarts[MyPlayerState->PlayerNumber];
	
	APlayerCharacter* CreatePawn = GetWorld()->SpawnActor<APlayerCharacter>(
		GameMode->DefaultPawnClass,
		GameStart->GetActorLocation(),
		GameStart->GetActorRotation()
		);
	Possess(CreatePawn);

	MyPlayerState->InfoUpdate();
}

void AMyPlayerController::RefreshPlayerInfo_Implementation(const TArray<FString>& PlayerName,const TArray<int32>& PlayerScore)
{
	FillHUDPlayerInfo(PlayerName,PlayerScore);
}

void AMyPlayerController::RefreshPlayerHP_Implementation(const TArray<FString>& PlayerName,const TArray<int32>& PlayerHP)
{
	FillHUDPlayerHP(PlayerName,PlayerHP);
}

void AMyPlayerController::NotifyPlayerSum_Implementation(int PlayerSum)
{
	RefreshPlayerSum(PlayerSum);
}

void AMyPlayerController::AskToStartGame_Implementation(int32 GameType)
{
	if(GameType != 0 && GameType != 1)return;
	
	if(!HasAuthority())return;
	
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);

	if(GameType == 0)
	{
		// 只有靶场模式需要检测人数
		// 人数不符合开始游戏的要求，只有在请求开始游戏阶段判断人数，后续等待时间如有玩家退出，不进行检测
		// TODO 此处还可以增加错误窗口，如果人数不够就有人点了开始，就提示客户端玩家人数数据异常
		if(GameMode->AllPlayerController.Num() < 2) return;
	}
	
	// 切换服务器游戏状态
	UMyGameInstance* ServerGameInstance = Cast<UMyGameInstance>(GameMode->GetGameInstance());
	if(!ServerGameInstance->IsCurrentState(EPlayerGameMode::WaitingPlayer))
	{
		UE_LOG(LogTemp,Warning,TEXT("Server State is not WaitingPlayer,Fail to Start Game"));
		return;
	}
	if(!ServerGameInstance->TransitionToState(EPlayerGameMode::WaitingPassing))
	{
		UE_LOG(LogTemp,Warning,TEXT("TransitionToState WaitingPassing Error"));
		return;
	}
	
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		UE_LOG(LogTemp,Warning,TEXT("准备开始游戏"));
		PlayerController->GetMessageToPass(GameMode->PassWaitTime,GameType);
	}

	PassTimeDelegate.BindUFunction(this,TEXT("Pass"),GameType);
	GetWorld()->GetTimerManager().SetTimer(PassTimeHandle,PassTimeDelegate,GameMode->PassWaitTime,false);

	if(GameType == 1)
	{
		for (auto PlayerController : GameMode->AllPlayerController)
		{
			PlayerController->GetMessageToHideHUDUI(GameType);
		}
	}
}

void AMyPlayerController::Pass_Implementation(int32 GameType)
{
	PassTimeDelegate.Unbind();
	GetWorld()->GetTimerManager().ClearTimer(PassTimeHandle);

	if(!HasAuthority())return;
	
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);
	
	// 切换服务器游戏状态
	UMyGameInstance* ServerGameInstance = Cast<UMyGameInstance>(GameMode->GetGameInstance());
	if(!ServerGameInstance->IsCurrentState(EPlayerGameMode::WaitingPassing))
	{
		UE_LOG(LogTemp,Warning,TEXT("Server State is not WaitingPassing,Fail to Pass"));
		return;
	}
	if(!ServerGameInstance->TransitionToState(EPlayerGameMode::PlayingReady))
	{
		UE_LOG(LogTemp,Warning,TEXT("TransitionToState PlayingReady Error"));
		return;
	}
	

	int32 i = 0;
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		if(GameType == 0)
		{
			// TODO 地图只有三个点位，此处默认人数不大于3，后续可以考虑将代码升级为自动生成点位
			// 将玩家传送到靶场中的点位
			if(GameMode->Target_PlayerPoints.Num() <= i)continue;
			PlayerController->GetPawn()->SetActorTransform(GameMode->Target_PlayerPoints[i]->GetTransform());
		}else if(GameType == 1)
		{
			// TODO 地图只有三个点位，此处默认人数不大于3，后续可以考虑将代码升级为自动生成点位
			// 将玩家传送到靶场中的点位
			if(GameMode->AI_PlayerPoints.Num() <= i)continue;
			PlayerController->GetPawn()->SetActorTransform(GameMode->AI_PlayerPoints[i]->GetTransform());
		}
		
		i++;
		PlayerController->GetMessageToReadyGame(GameMode->StartGameWaitTime);
	}

	// 重置分数
	GameMode->ClearPlayerScore();
	// 重置血量
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		ABaseCharacter* BasePawn = Cast<ABaseCharacter>(PlayerController->GetPawn());
		BasePawn->ResumePlayerHP();
	}
	
	GameMode->ChangeFireAbility(false);
	
	if(GameType == 1)
	{
		AskToSpawnAIPawn();
	}
	
	PassTimeDelegate.BindUFunction(this,TEXT("StartGame"),GameType);
	GetWorld()->GetTimerManager().SetTimer(PassTimeHandle,PassTimeDelegate,GameMode->StartGameWaitTime,false);
}

void AMyPlayerController::StartGame_Implementation(int32 GameType)
{
	PassTimeDelegate.Unbind();
	GetWorld()->GetTimerManager().ClearTimer(PassTimeHandle);

	if(!HasAuthority())return;
	
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);
	
	// 切换服务器游戏状态
	UMyGameInstance* ServerGameInstance = Cast<UMyGameInstance>(GameMode->GetGameInstance());
	if(!ServerGameInstance->IsCurrentState(EPlayerGameMode::PlayingReady))
	{
		UE_LOG(LogTemp,Warning,TEXT("Server State is not PlayingReady,Fail to Playing"));
		return;
	}

	if(GameType == 0)
	{
		if(!ServerGameInstance->TransitionToState(EPlayerGameMode::PlayingTarget))
		{
			UE_LOG(LogTemp,Warning,TEXT("TransitionToState PlayingTarget Error"));
			return;
		}
	}
	else if(GameType == 1)
	{
		if(!ServerGameInstance->TransitionToState(EPlayerGameMode::PlayingAI))
		{
			UE_LOG(LogTemp,Warning,TEXT("TransitionToState PlayingAI Error"));
			return;
		}
	}
	
	GameMode->ChangeFireAbility(true);

	UE_LOG(LogTemp,Warning,TEXT("测试使用：正式开始游戏!!"));

	// 靶场游戏有倒计时作为结束条件
	if(GameType == 0)
	{
		// 客户端显示游戏倒计时
		for (auto PlayerController : GameMode->AllPlayerController)
		{
			PlayerController->GetMessageToStartGame(GameMode->TotalGameTime);
		}
		
		GetWorld()->GetTimerManager().SetTimer(PassTimeHandle,this,&AMyPlayerController::EndGame,GameMode->TotalGameTime,false);
	}

	// TODO AI场游戏结束条件未定
	// -----
}

void AMyPlayerController::EndGame_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(PassTimeHandle);

	if(!HasAuthority())return;

	UE_LOG(LogTemp,Warning,TEXT("测试使用：靶场游戏结束!!"));
	// 计算分数
	CalculateScore();
}

void AMyPlayerController::CalculateScore_Implementation()
{
	if(!HasAuthority())return;

	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);

	TArray<FString> PlayerNumber;
	TArray<int32> PlayerScore;
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerController->PlayerState);
		PlayerNumber.Add(MyPlayerState->PlayerName);
		PlayerScore.Add(MyPlayerState->PlayerScore);
	}

	UE_LOG(LogTemp,Warning,TEXT("测试使用：计算分数!!"));
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		PlayerController->GetMessageToCalScore(PlayerNumber,PlayerScore);
	}
}

void AMyPlayerController::AskToBackWait_Implementation()
{
	if(!HasAuthority())return;
	
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);
	
	// 切换服务器游戏状态
	UMyGameInstance* ServerGameInstance = Cast<UMyGameInstance>(GameMode->GetGameInstance());
	if(!ServerGameInstance->IsCurrentState(EPlayerGameMode::PlayingAIEnd) && !ServerGameInstance->IsCurrentState(EPlayerGameMode::PlayingTarget))
	{
		UE_LOG(LogTemp,Warning,TEXT("Server State is not PlayingAI or PlayingTarget,Fail to WaitingPlayer"));
		return;
	}
	if(!ServerGameInstance->TransitionToState(EPlayerGameMode::WaitingPlayer))
	{
		UE_LOG(LogTemp,Warning,TEXT("TransitionToState WaitingPlayer Error"));
		return;
	}

	GameMode->ClearPlayerScore();
	
	int32 i = 0;
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		UE_LOG(LogTemp,Warning,TEXT("回到等待区域"));
		// TODO 地图只有三个点位，此处默认人数不大于4，后续可以考虑将代码升级为自动生成点位
		// 将玩家传送到等待区中的点位
		if(GameMode->GetGameStarts.Num() <= i)continue;
		PlayerController->GetPawn()->SetActorTransform(GameMode->GetGameStarts[i]->GetTransform());
		i++;

		ABaseCharacter* GetPawn = Cast<ABaseCharacter>(PlayerController->GetPawn());
		GetPawn->ResumePlayerHP();
		PlayerController->GetMessageToBackWait();

		// 通知所有玩家当前玩家数
		PlayerController->NotifyPlayerSum(GameMode->AllPlayerController.Num());
	}
}


void AMyPlayerController::GetMessageToPass_Implementation(int32 WaitTime,int32 GameType)
{
	SendReadyPassMessageToUMG(WaitTime,GameType);
}

void AMyPlayerController::GetMessageToReadyGame_Implementation(int32 WaitTime)
{
	SendReadyGameMessageToUMG(WaitTime);
}

void AMyPlayerController::GetMessageToStartGame_Implementation(int32 TotalGameTime)
{
	SendStartGameMessageToUMG(TotalGameTime);
}

void AMyPlayerController::GetMessageToCalScore_Implementation(const TArray<FString>& PlayerNumber,const TArray<int32>& PlayerScore)
{
	SendCalScoreMessageToUMG(PlayerNumber,PlayerScore);
}

void AMyPlayerController::GetMessageToHideHUDUI_Implementation(int32 GameType)
{
	SendHideHUDUIMessageToUMG(GameType);
}

void AMyPlayerController::GetMessageToBackWait_Implementation()
{
	SendBackWaitMessageToUMG();
}

void AMyPlayerController::AskToSpawnAIPawn_Implementation()
{
	if (!HasAuthority())return;

	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);

	GameMode->RespawnAIPawn();
}





