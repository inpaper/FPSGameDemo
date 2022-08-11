// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyCharacter.h"
#include "MyGameInstance.h"
#include "MyPlayerState.h"
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
	
	AMyCharacter* CreatePawn = GetWorld()->SpawnActor<AMyCharacter>(
		GameMode->DefaultPawnClass,
		GameStart->GetActorLocation(),
		GameStart->GetActorRotation()
		);
	Possess(CreatePawn);
}

void AMyPlayerController::RefreshPlayerInfo_Implementation(const TArray<int32>& PlayerNumber,const TArray<int32>& PlayerScore)
{
	FillHUDPlayerInfo(PlayerNumber,PlayerScore);
}

void AMyPlayerController::NotifyPlayerSum_Implementation(int PlayerSum)
{
	RefreshPlayerSum(PlayerSum);
}

void AMyPlayerController::AskToStartGame_Implementation()
{
	if(!HasAuthority())return;
	
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);

	// 人数不符合开始游戏的要求，只有在请求开始游戏阶段判断人数，后续等待时间如有玩家退出，不进行检测
	// TODO 此处还可以增加错误窗口，如果人数不够就有人点了开始，就提示客户端玩家人数数据异常
	if(GameMode->AllPlayerController.Num() < 2) return;
	
	// 切换服务器游戏状态
	UMyGameInstance* ServerGameInstance = Cast<UMyGameInstance>(GameMode->GetGameInstance());
	if(!ServerGameInstance->IsCurrentState(EGameState::WaitingPlayer))
	{
		UE_LOG(LogTemp,Warning,TEXT("Server State is not WaitingPlayer,Fail to Start Game"));
		return;
	}
	if(!ServerGameInstance->TransitionToState(EGameState::WaitingPassing))
	{
		UE_LOG(LogTemp,Warning,TEXT("TransitionToState WaitingPassing Error"));
		return;
	}
	
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		UE_LOG(LogTemp,Warning,TEXT("准备开始游戏"));
		PlayerController->GetMessageToPass(GameMode->PassWaitTime);
	}
	
	GetWorld()->GetTimerManager().SetTimer(PassTimeHandle,this,&AMyPlayerController::Pass,GameMode->PassWaitTime,false);
	
}

void AMyPlayerController::Pass_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(PassTimeHandle);

	if(!HasAuthority())return;
	
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);
	
	// 切换服务器游戏状态
	UMyGameInstance* ServerGameInstance = Cast<UMyGameInstance>(GameMode->GetGameInstance());
	if(!ServerGameInstance->IsCurrentState(EGameState::WaitingPassing))
	{
		UE_LOG(LogTemp,Warning,TEXT("Server State is not WaitingPassing,Fail to Pass"));
		return;
	}
	if(!ServerGameInstance->TransitionToState(EGameState::PlayingReady))
	{
		UE_LOG(LogTemp,Warning,TEXT("TransitionToState PlayingReady Error"));
		return;
	}
	
	// TODO 地图只有三个点位，此处默认人数不大于3，后续可以考虑将代码升级为自动生成点位
	int32 i = 0;
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		// 将玩家传送到靶场中的点位
		PlayerController->GetPawn()->SetActorTransform(GameMode->GetShootingTransform[i]->GetTransform());
		i++;

		PlayerController->GetMessageToReadyGame(GameMode->StartGameWaitTime);
	}

	GameMode->ClearPlayerScore();
	GameMode->ChangeFireAbility(false);
	
	GetWorld()->GetTimerManager().SetTimer(PassTimeHandle,this,&AMyPlayerController::StartGame,GameMode->StartGameWaitTime,false);
}

void AMyPlayerController::StartGame_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(PassTimeHandle);

	if(!HasAuthority())return;
	
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);
	
	// 切换服务器游戏状态
	UMyGameInstance* ServerGameInstance = Cast<UMyGameInstance>(GameMode->GetGameInstance());
	if(!ServerGameInstance->IsCurrentState(EGameState::PlayingReady))
	{
		UE_LOG(LogTemp,Warning,TEXT("Server State is not PlayingReady,Fail to Playing"));
		return;
	}
	if(!ServerGameInstance->TransitionToState(EGameState::Playing))
	{
		UE_LOG(LogTemp,Warning,TEXT("TransitionToState Playing Error"));
		return;
	}

	GameMode->ChangeFireAbility(true);

	UE_LOG(LogTemp,Warning,TEXT("测试使用：正式开始游戏!!"));
	// 客户端显示游戏倒计时
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		PlayerController->GetMessageToStartGame(GameMode->TotalGameTime);
	}

	GetWorld()->GetTimerManager().SetTimer(PassTimeHandle,this,&AMyPlayerController::EndGame,GameMode->TotalGameTime,false);
}

void AMyPlayerController::EndGame_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(PassTimeHandle);

	if(!HasAuthority())return;

	UE_LOG(LogTemp,Warning,TEXT("测试使用：游戏结束!!"));
	// 计算分数
	CalculateScore();
}

void AMyPlayerController::CalculateScore_Implementation()
{
	if(!HasAuthority())return;

	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);

	TArray<int32> PlayerNumber;
	TArray<int32> PlayerScore;
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerController->PlayerState);
		PlayerNumber.Add(MyPlayerState->PlayerNumber);
		PlayerScore.Add(MyPlayerState->PlayerScore);
	}

	UE_LOG(LogTemp,Warning,TEXT("测试使用：计算分数!!"));
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		PlayerController->GetMessageToCalScore(PlayerNumber,PlayerScore);
	}
}



void AMyPlayerController::GetMessageToPass_Implementation(int32 WaitTime)
{
	SendReadyPassMessageToUMG(WaitTime);
}

void AMyPlayerController::GetMessageToReadyGame_Implementation(int32 WaitTime)
{
	SendReadyGameMessageToUMG(WaitTime);
}

void AMyPlayerController::GetMessageToStartGame_Implementation(int32 TotalGameTime)
{
	SendStartGameMessageToUMG(TotalGameTime);
}

void AMyPlayerController::GetMessageToCalScore_Implementation(const TArray<int32>& PlayerNumber,const TArray<int32>& PlayerScore)
{
	SendCalScoreMessageToUMG(PlayerNumber,PlayerScore);
}






