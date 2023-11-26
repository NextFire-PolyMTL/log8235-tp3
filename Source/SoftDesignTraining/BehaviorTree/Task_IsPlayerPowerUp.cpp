// Fill out your copyright notice in the Description page of Project Settings.


#include "Task_IsPlayerPowerUp.h"
#include "../SDTAIController.h"
#include "../SoftDesignTrainingMainCharacter.h"


EBTNodeResult::Type UIsPlayerPowerUp::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(ownerComp.GetAIOwner());
    if (aiController == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    ASoftDesignTrainingMainCharacter* player = Cast<ASoftDesignTrainingMainCharacter>(aiController->GetFocusActor());
    if (player == nullptr)
    {
        return EBTNodeResult::Failed;
    }

	return player->IsPoweredUp() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
