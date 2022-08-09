// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "MyCharacter.h"
#include "MyPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "FPSGameDemo/FPSGameDemoGameModeBase.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"

void AMyPlayerController::OwnerClientPostLogin_Implementation()
{
	// UE_LOG(LogTemp,Warning,TEXT("is client %s"),IsLocalController() ? TEXT("True") : TEXT("True"));
	// UE_LOG(LogTemp,Warning,TEXT("ROLE_Authority %s"),GetLocalRole() == ROLE_Authority ? TEXT("True") : TEXT("True"));
	UE_LOG(LogTemp,Warning,TEXT("服务器 %s"),HasAuthority() ? TEXT("True") : TEXT("True"));

	// 蓝图中处理UMG事件 TODO 玩家分数还没有添加上去
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



