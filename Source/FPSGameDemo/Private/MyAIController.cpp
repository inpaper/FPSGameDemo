// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "PlayerCharacter.h"
#include "FPSGameDemo/FPSGameDemoGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


void AMyAIController::Tick(float DeltaSeconds)
{
	// if(GetPawn() && HasAuthority())
	// {
	// 	AIBehavior();
	// }
	
}

void AMyAIController::Init()
{
	if(GameMode == nullptr)
	{
		GameMode = Cast<AFPSGameDemoGameModeBase>(UGameplayStatics::GetGameMode(this));
		ControlPawn = Cast<AAICharacter>(GetPawn());
	}
}

// void AMyAIController::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
// {
// 	DOREPLIFETIME_CONDITION( AMyAIController, GetLookForward, COND_SimulatedOnly );
// }

void AMyAIController::AIBehavior()
{
	if(GameMode == nullptr)
		Init();

	AActor* NearlyPlayer = FindNearlyPlayer();
	if(NearlyPlayer == nullptr)return;

	LookToNearlyPlayer(NearlyPlayer);
}

AActor* AMyAIController::FindNearlyPlayer()
{
	if(GameMode == nullptr)
		Init();
	
	AMyPlayerController* NearlyPlayer = nullptr;
	// FLT_MAX为浮点最大值
	float MinDistance = FLT_MAX;
	
	// 检查120度视角内的所有玩家并朝最近的玩家射击
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		// 在有人机的情况下进入 存在只有玩家控制器还没有Pawn的情况
		if(PlayerController->GetPawn() == nullptr)continue;

		FVector PlayerLocation = PlayerController->GetPawn()->GetActorLocation();
		FVector AILocation = ControlPawn->GetActorLocation();

		// 检查玩家和人机之前是否有障碍物，有障碍物就不讨论
		FHitResult HitResult;
		if(GetWorld()->LineTraceSingleByChannel(HitResult,AILocation,PlayerLocation,ECollisionChannel::ECC_Visibility))
			continue;

		FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(AILocation,PlayerLocation);
		
		const float Distance = FVector::Distance(AILocation,PlayerLocation);
		if(FMath::Abs(ControlPawn->GetActorRotation().Yaw - LookRot.Yaw) < AI_AngleOfView / 2 && MinDistance > Distance)
		{
			NearlyPlayer = PlayerController;
			MinDistance = Distance;
		}
	}

	if(NearlyPlayer == nullptr)
		return nullptr;
	
	return NearlyPlayer->GetPawn();
}

void AMyAIController::LookToNearlyPlayer(AActor* const NearlyPlayer)
{
	if(GameMode == nullptr)
		Init();
	
	FRotator AIRot = ControlPawn->GetActorRotation();
	FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(
		ControlPawn->GetActorLocation(),NearlyPlayer->GetActorLocation());
	
	float ChangeYaw = LookRot.Yaw - AIRot.Yaw;
	// 防止在+180和-180的边界处无法正常旋转
	if(ChangeYaw > 180)
	{
		ChangeYaw -= 360;
	}else if(ChangeYaw < -180)
	{
		ChangeYaw += 360;
	}

	float RelativeSpeed = FMath::Clamp<float>(ChangeYaw,-1,1);
	float RotationChange = RelativeSpeed * MaxDegreePerSecond * GetWorld()->DeltaTimeSeconds;
	float NewRotationYaw = AIRot.Yaw + RotationChange;
	
	// 转身运动
	ControlPawn->SetActorRelativeRotation(FRotator(0,NewRotationYaw,0));
}

bool AMyAIController::CheckNearlyPlayerRot(AActor* const NearlyPlayer, float LookPrecision)
{
	if(GameMode == nullptr)
		Init();
	
	FRotator AIRot = ControlPawn->GetActorRotation();
	FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(
		ControlPawn->GetActorLocation(),NearlyPlayer->GetActorLocation());
	
	float ChangeYaw = LookRot.Yaw - AIRot.Yaw;

	return FMath::Abs(ChangeYaw) < LookPrecision;
}


void AMyAIController::GetPlayerPawnTransform()
{
	if(!HasAuthority())return;
	
	const FVector OwnerPawnLocation = ControlPawn->GetActorLocation();
	const FRotator OwnerPawnRot = ControlPawn->GetActorRotation();
	
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

	// TODO 无法实现枪跟随目标高度进行改变 无法同步到其他客户端去
	float LookForward = FVector::DotProduct(AimToDirection,ControlPawn->GetActorUpVector());
	// ControlPawn->GetLookForward = LookForward;
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
	ControlPawn->SetActorRelativeRotation(FRotator(0,NewRotationYaw,0));

	if(FMath::Abs(ChangeYaw) < 0.3f)
	{
		// ControlPawn->AIFire();
	}
}

// TODO NetMulticast 发送不到其他有AIController的客户端
void AMyAIController::NotifyClientsLookForward_Implementation(float LookForward)
{
	UE_LOG(LogTemp,Warning,TEXT("Other Client Get"));
	APawn* OwnerPawn = GetPawn();
	APlayerCharacter* MyCharacter = Cast<APlayerCharacter>(OwnerPawn);
	if(MyCharacter == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("AIController Don't Possess MyCharacter"));
		return;
	}
	UE_LOG(LogTemp,Warning,TEXT("MyCharacter->GetLookForward %f %f"),MyCharacter->GetLookForward,LookForward);
	MyCharacter->GetLookForward = LookForward;
}

