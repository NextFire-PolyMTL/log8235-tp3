// Fill out your copyright notice in the Description page of Project Settings.


#include "Task_IsValid.h"


EBTNodeResult::Type UIsValid::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	// TODO: Check if BlackboardKey is a vector and is different than FAISystem::InvalidLocation.
	return EBTNodeResult::Succeeded;
}
