// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorator_IsCollectibleCooldown.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "./SDTCollectible.h"
#include "./SDTCollectible.h"
#include "./SoftDesignTrainingMainCharacter.h"
#include "./SDTAIController.h"

UDecorator_IsCollectibleCooldown::UDecorator_IsCollectibleCooldown(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Is Collectible On Cooldown";
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UDecorator_IsCollectibleCooldown, BlackboardKey), AActor::StaticClass());
}

bool UDecorator_IsCollectibleCooldown::CalculateRawConditionValue(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) const
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

	ASDTCollectible* collectible = Cast<ASDTCollectible>(blackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID()));
	if (collectible == nullptr)
	{
		return false;
	}

	return !collectible->IsOnCooldown();
}


EBlackboardNotificationResult UDecorator_IsCollectibleCooldown::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
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