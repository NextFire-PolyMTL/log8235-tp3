// Fill out your copyright notice in the Description page of Project Settings.


#include "Task_MoveToFocus.h"


EBTNodeResult::Type UMoveToFocus::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	// TODO: Retrieve the focused actor from the AAIController object. If the focused actor is an ACharacter, call MoveTo, otherwise call MoveToLocation.
	// TODO: We have to avoid to call again and again at each tick the MoveTo or MoveToLocation. Adding a check for the target point on the pathfollowingcomponent may work?
	return EBTNodeResult::Succeeded;
}
