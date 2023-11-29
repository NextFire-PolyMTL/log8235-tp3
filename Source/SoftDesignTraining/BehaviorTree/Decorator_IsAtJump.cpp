// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorator_IsAtJump.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../SDTAIController.h"
#include "../SoftDesignTrainingMainCharacter.h"


UDecorator_IsAtJump::UDecorator_IsAtJump(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Is Jumping";
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDecorator_IsAtJump, BlackboardKey), AActor::StaticClass());
}

bool UDecorator_IsAtJump::CalculateRawConditionValue(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) const
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

	ASDTAIController* aiController = Cast<ASDTAIController>(ownerComp.GetAIOwner());
	if (aiController == nullptr) {
		return false;
	}

	return aiController->AtJumpSegment;
}


EBlackboardNotificationResult UDecorator_IsAtJump::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
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

