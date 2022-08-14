// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "MyCharacter.h"
#include "FPSGameDemo/FPSGameDemoGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


void AMyAIController::Tick(float DeltaSeconds)
{
	if(GetPawn())
	{
		GetPlayerPawnTransform();
	}
}

// void AMyAIController::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
// {
// 	DOREPLIFETIME_CONDITION( AMyAIController, GetLookForward, COND_SimulatedOnly );
// }

void AMyAIController::GetPlayerPawnTransform()
{
	if(!HasAuthority())return;

	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);
	APawn* OwnerPawn = GetPawn();

	const FVector OwnerPawnLocation = OwnerPawn->GetActorLocation();
	const FRotator OwnerPawnRot = OwnerPawn->GetActorRotation();
	
	// 先获取最近的单位的位置，朝他移动
	// TODO AI性格射击的时候可能需要获取对应的旋转度
	FVector MinMoveToLocation;
	// FLT_MAX为浮点最大值
	float MinDistance = FLT_MAX;
	
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		// 在有人机的情况下进入 存在只有玩家控制器还没有Pawn的情况
		if(PlayerController->GetPawn() == nullptr)continue;
		
		FVector MoveToLocation = PlayerController->GetPawn()->GetActorLocation();
		float Distance = FVector::Distance(OwnerPawnLocation,MoveToLocation);
		if(MinDistance > Distance)
		{
			MinDistance = Distance;
			MinMoveToLocation = MoveToLocation;
		}
	}
	// 先转身 再移动
	FRotator EndRot = UKismetMathLibrary::FindLookAtRotation(OwnerPawnLocation,MinMoveToLocation);

	AimToDirection = (MinMoveToLocation - OwnerPawnLocation).GetSafeNormal();
	AMyCharacter* MyCharacter = Cast<AMyCharacter>(OwnerPawn);
	if(MyCharacter == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("AIController Don't Possess MyCharacter"));
		return;
	}

	// TODO 无法实现枪跟随目标高度进行改变 无法同步到其他客户端去
	float LookForward = FVector::DotProduct(AimToDirection,OwnerPawn->GetActorUpVector());
	MyCharacter->GetLookForward = LookForward;
	// NotifyClientsLookForward(LookForward);
	
	float ChangeYaw = EndRot.Yaw - OwnerPawnRot.Yaw;
	// float ChangePitch = EndRot.Pitch - OwnerPawnRot.Pitch;
	if(ChangeYaw > 180)
	{
		ChangeYaw -= 360;
	}else if(ChangeYaw < -180)
	{
		ChangeYaw += 360;
	}

	float RelativeSpeed = FMath::Clamp<float>(ChangeYaw,-1,1);
	float RotationChange = RelativeSpeed * MaxDegreePerSecond * GetWorld()->DeltaTimeSeconds;
	float NewRotationYaw = OwnerPawnRot.Yaw + RotationChange;
	
	// 转身运动
	OwnerPawn->SetActorRelativeRotation(FRotator(0,NewRotationYaw,0));

	if(FMath::Abs(ChangeYaw) < 0.3f)
	{
		MyCharacter->AIFire();
	}
}

// TODO NetMulticast 发送不到其他有AIController的客户端
void AMyAIController::NotifyClientsLookForward_Implementation(float LookForward)
{
	UE_LOG(LogTemp,Warning,TEXT("Other Client Get"));
	APawn* OwnerPawn = GetPawn();
	AMyCharacter* MyCharacter = Cast<AMyCharacter>(OwnerPawn);
	if(MyCharacter == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("AIController Don't Possess MyCharacter"));
		return;
	}
	UE_LOG(LogTemp,Warning,TEXT("MyCharacter->GetLookForward %f %f"),MyCharacter->GetLookForward,LookForward);
	MyCharacter->GetLookForward = LookForward;
}


