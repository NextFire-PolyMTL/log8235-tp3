// Fill out your copyright notice in the Description page of Project Settings.


#include "Task_IsBBValueTrue.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UIsBBValueTrue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const UBlackboardComponent* myBlackboard = OwnerComp.GetBlackboardComponent())
	{
		return BlackboardKey.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass() && myBlackboard->GetValue<UBlackboardKeyType_Bool>(BlackboardKey.GetSelectedKeyID()) ?
			EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
