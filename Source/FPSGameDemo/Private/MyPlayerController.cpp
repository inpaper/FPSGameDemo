// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "MyCharacter.h"
#include "MyPlayerState.h"
// #include "Blueprint/UserWidget.h"
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

// TODO 不清楚为什么会出现第二个屏幕不更新的情况 此处暂时先注释AMyPlayerState::ScoreChange_Implementation()
// 使用蓝图版本
void AMyPlayerController::RefreshPlayerInfo_Implementation(const TArray<FPlayerInfo>& AllPlayerState)
{
	UE_LOG(LogTemp,Warning,TEXT("RefreshPlayerInfo_Implementation"));
	
	ClearHUDPlayerInfo();
	for (auto GetPlayerState : AllPlayerState)
	{
		FillHUDPlayerInfo(GetPlayerState.Name,GetPlayerState.Score);
	}
}

void AMyPlayerController::NotifyPlayerSum_Implementation(int PlayerSum)
{
	RefreshPlayerSum(PlayerSum);
}

void AMyPlayerController::AskToStartGame_Implementation()
{
	if(HasAuthority())
	{
		AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
		AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);

		// 人数不符合开始游戏的要求
		// TODO 此处还可以增加错误窗口，如果人数不够就有人点了开始，就提示客户端玩家人数数据异常
		if(GameMode->AllPlayerController.Num() < 2) return;

		for (auto PlayerController : GameMode->AllPlayerController)
		{
			UE_LOG(LogTemp,Warning,TEXT("准备开始游戏"));
			PlayerController->GetMessageToPass(GameMode->PassWaitTime);
		}
		
		GetWorld()->GetTimerManager().SetTimer(PassTimeHandle,this,&AMyPlayerController::Pass,GameMode->PassWaitTime,false);
	}
}

void AMyPlayerController::Pass_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(PassTimeHandle);
	
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);

	// 人数不符合开始游戏的要求
	// TODO 此处还可以增加错误窗口，如果人数不够就有人点了开始，就提示客户端玩家人数数据异常
	if(GameMode->AllPlayerController.Num() < 2) return;

	// TODO 地图只有三个点位，此处默认人数不大于3，后续可以考虑将代码升级为自动生成点位
	int32 i = 0;
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		// 将玩家传送到靶场中的点位
		PlayerController->GetPawn()->SetActorTransform(GameMode->GetShootingTransform[i]->GetTransform());
		i++;

		PlayerController->GetMessageToReadyGame(GameMode->StartGameWaitTime);
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






