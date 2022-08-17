// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "PlayerCharacter.h"
#include "FPSGameDemo/FPSGameDemoGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void AMyPlayerState::InitPlayerNumber(int const PlayerNumberToSet)
{
	if(HasAuthority())
	{
		PlayerNumber = PlayerNumberToSet;
		PlayerName += FString::FromInt(PlayerNumberToSet);
	}
}


void AMyPlayerState::AddScore(int32 const GetScore)
{
	if(HasAuthority())
	{
		PlayerScore += GetScore;
		InfoUpdate();
	}
}

void AMyPlayerState::DecreaseScore(int32 const GetScore)
{
	if(HasAuthority())
	{
		PlayerScore -= GetScore;
		InfoUpdate();
	}
}

void AMyPlayerState::SetPlayerScore(int32 const GetScore)
{
	if(HasAuthority())
	{
		PlayerScore = GetScore;
		InfoUpdate();
	}
}


void AMyPlayerState::ResetScore()
{
	if(HasAuthority())
	{
		PlayerScore = 0;
		InfoUpdate();
	}
}

void AMyPlayerState::InfoUpdate_Implementation()
{
	if(!HasAuthority())return;
	
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);
	
	TArray<FString> PlayerNumberArray;
	TArray<int32> PlayerScoreArray;
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerController->PlayerState);
		PlayerNumberArray.Add(MyPlayerState->PlayerName);
		PlayerScoreArray.Add(MyPlayerState->PlayerScore);
	}
	
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		// 更新积分榜信息
		PlayerController->RefreshPlayerInfo(PlayerNumberArray,PlayerScoreArray);

		// 更新用户名称
		if(PlayerController->GetPawn() == nullptr)return;
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());
		if(PlayerCharacter == nullptr)return;
		PlayerCharacter->GetMessageToRefreshHPName(Cast<AMyPlayerState>(PlayerController->PlayerState)->PlayerName);
	}
}

void AMyPlayerState::ChangeName_Implementation(const FString& GetName)
{
	if(!HasAuthority())return;

	PlayerName = GetName;
	InfoUpdate();
}


