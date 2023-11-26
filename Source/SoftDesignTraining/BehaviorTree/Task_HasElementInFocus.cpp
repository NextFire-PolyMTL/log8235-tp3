// Fill out your copyright notice in the Description page of Project Settings.


#include "Task_HasElementInFocus.h"
#include "GameFramework/Character.h"
#include "../SDTAIController.h"


EBTNodeResult::Type UHasElementInFocus::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	auto aiController = ownerComp.GetAIOwner();
	if (aiController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	auto focusedActor = aiController->GetFocusActor();
	return MustBeCharacterActor && Cast<ACharacter>(focusedActor) != nullptr || !MustBeCharacterActor && focusedActor != nullptr ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
