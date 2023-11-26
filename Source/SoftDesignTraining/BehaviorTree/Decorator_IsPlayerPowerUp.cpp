// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorator_IsPlayerPowerUp.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../SDTAIController.h"
#include "../SoftDesignTrainingMainCharacter.h"


UDecorator_IsPlayerPowerUp::UDecorator_IsPlayerPowerUp(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Is Player Power Up";
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDecorator_IsPlayerPowerUp, BlackboardKey), AActor::StaticClass());
}

bool UDecorator_IsPlayerPowerUp::CalculateRawConditionValue(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) const
{
	UBlackboardComponent* blackboard = ownerComp.GetBlackboardComponent();
	if (blackboard == nullptr)
	{
		return false;
	}

	if (BlackboardKey.SelectedKeyType != UBlackboardKeyType_Object::StaticClass())
	{
		return false;
	}

	ASoftDesignTrainingMainCharacter* player = Cast<ASoftDesignTrainingMainCharacter>(blackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID()));
	if (player == nullptr)
	{
		return false;
	}

	return player->IsPoweredUp();
}


EBlackboardNotificationResult UDecorator_IsPlayerPowerUp::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = (UBehaviorTreeComponent*)Blackboard.GetBrainComponent();
	if (BehaviorComp == nullptr)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if (BlackboardKey.GetSelectedKeyID() == ChangedKeyID)
	{
		ConditionalFlowAbort(*BehaviorComp, EBTDecoratorAbortRequest::ConditionResultChanged);
	}

	return EBlackboardNotificationResult::ContinueObserving;
}