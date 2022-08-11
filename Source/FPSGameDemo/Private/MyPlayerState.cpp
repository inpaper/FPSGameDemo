// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "FPSGameDemo/FPSGameDemoGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void AMyPlayerState::InitPlayerNumber(int const PlayerNumberToSet)
{
	if(HasAuthority())
	{
		PlayerNumber = PlayerNumberToSet;
	}
}


void AMyPlayerState::AddScore(int32 const GetScore)
{
	if(HasAuthority())
	{
		PlayerScore += GetScore;
		ScoreUpdate();
	}
}

void AMyPlayerState::DecreaseScore(int32 const GetScore)
{
	if(HasAuthority())
	{
		PlayerScore -= GetScore;
		ScoreUpdate();
	}
}

void AMyPlayerState::SetPlayerScore(int32 const GetScore)
{
	if(HasAuthority())
	{
		PlayerScore = GetScore;
		ScoreUpdate();
	}
}


void AMyPlayerState::ResetScore()
{
	if(HasAuthority())
	{
		PlayerScore = 0;
		ScoreUpdate();
	}
}

void AMyPlayerState::ScoreUpdate_Implementation()
{
	if(!HasAuthority())return;
	
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);
	
	TArray<int32> PlayerNumberArray;
	TArray<int32> PlayerScoreArray;
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerController->PlayerState);
		PlayerNumberArray.Add(MyPlayerState->PlayerNumber);
		PlayerScoreArray.Add(MyPlayerState->PlayerScore);
	}
	
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		PlayerController->RefreshPlayerInfo(PlayerNumberArray,PlayerScoreArray);
	}
}


