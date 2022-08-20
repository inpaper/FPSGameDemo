// Copyright Epic Games, Inc. All Rights Reserved.


#include "FPSGameDemoGameModeBase.h"
#include "MyAIController.h"
#include "MyCharacter.h"
#include "MyGameInstance.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "PlayerCharacter.h"
#include "ShootingPawnTransform.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AFPSGameDemoGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	// 服务器特有功能
	if(HasAuthority())
	{
		UE_LOG(LogTemp,Warning,TEXT("NewPlayer Name %s"),*NewPlayer->GetName());

		// 初次开启服务器，先获取地图上的所有点信息
		if(bCreateRoom)
		{
			bCreateRoom = false;
			
			UGameplayStatics::GetAllActorsOfClass(this,APlayerStart::StaticClass(),GetGameStarts);
			UE_LOG(LogTemp,Warning,TEXT("GameStart have %i Points"),GetGameStarts.Num());

			TArray<AActor*> GetAllPoints;
			UGameplayStatics::GetAllActorsOfClass(this,AShootingPawnTransform::StaticClass(),GetAllPoints);
			for (auto Point : GetAllPoints)
			{
				AShootingPawnTransform* GetPoint = Cast<AShootingPawnTransform>(Point);
				switch (GetPoint->RespawnPawnPoint)
				{
					case ERespawnPawnPoint::Target_PlayerPoint:
						Target_PlayerPoints.AddUnique(GetPoint);
						break;
					case ERespawnPawnPoint::AI_PlayerPoint:
						AI_PlayerPoints.AddUnique(GetPoint);
						break;
					case ERespawnPawnPoint::AI_AIPoint:
						AI_AIPoints.AddUnique(GetPoint);
						break;
					default:
						break;
				}
			}
			// UE_LOG(LogTemp,Warning,TEXT("GetShootingTransform have %i Points"),GetShootingTransform.Num());
		}
		
		AMyPlayerController* NewPlayerController = Cast<AMyPlayerController>(NewPlayer);
		AllPlayerController.AddUnique(NewPlayerController);
		// 保证玩家名称序号从1开始
		AMyPlayerState* MyPlayerState = NewPlayer->GetPlayerState<AMyPlayerState>();
		if(MyPlayerState == nullptr)
		{
			UE_LOG(LogTemp,Warning,TEXT("%s can't get PlayerState"),*NewPlayer->GetName());
			return;
		}
		MyPlayerState->InitPlayerNumber(AllPlayerController.Num());

		// 切换服务器游戏状态
		UMyGameInstance* ServerGameInstance = Cast<UMyGameInstance>(GetGameInstance());
		if(ServerGameInstance->IsCurrentState(EPlayerGameMode::MainMenu))
		{
			if(!ServerGameInstance->TransitionToState(EPlayerGameMode::WaitingPlayer))
			{
				UE_LOG(LogTemp,Warning,TEXT("TransitionToState WaitingPlayer Error"));
			}
		}
		
		// 执行拥有该控制器的客户端需要执行的方法
		NewPlayerController->OwnerClientPostLogin();
		
		MyPlayerState->InfoUpdate();
		
		// 通知所有玩家控制器，当前玩家数目
		for (auto PlayerController : AllPlayerController)
		{
			PlayerController->NotifyPlayerSum(AllPlayerController.Num());
		}
	}
}

void AFPSGameDemoGameModeBase::RespawnPlayerPawn_Implementation(APlayerController* PlayerController)
{
	APawn* ControlledPawn = PlayerController->GetPawn();
	if(ControlledPawn != nullptr)
	{
		ControlledPawn->Destroy();
	}
	APlayerCharacter* CreatePawn = GetWorld()->SpawnActor<APlayerCharacter>(
		DefaultPawnClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator
		);
	if(CreatePawn == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("Create Default Pawn Error"));
		return;
	}
	PlayerController->Possess(CreatePawn);
}

void AFPSGameDemoGameModeBase::ClearPlayerScore()
{
	for (auto PlayerController : AllPlayerController)
	{
		AMyPlayerState* GetPlayerState = PlayerController->GetPlayerState<AMyPlayerState>();
		GetPlayerState->ResetScore();
	}
}

void AFPSGameDemoGameModeBase::ChangeFireAbility(bool GetFireAbility)
{
	bFireAbility = GetFireAbility;
}

void AFPSGameDemoGameModeBase::RespawnAIPawn()
{
	if(!bFirstRespawnAI)
		return;
	
	bFirstRespawnAI = false;
	for (auto Point : AI_AIPoints)
	{
		AAICharacter* CreatePawn = GetWorld()->SpawnActor<AAICharacter>(
			AIPawnClass,
			Point->GetActorLocation(),
			Point->GetActorRotation()
		);
	
		// 在此处把AI的标签去掉，保证AI不会攻击AI
		CreatePawn->Tags.Empty();
		
		AMyAIController* AIController = Cast<AMyAIController>(CreatePawn->GetController());
		if(AIController == nullptr)
		{
			UE_LOG(LogTemp,Warning,TEXT("AIController Nullptr"));
			return;
		}
	}
}
