#include "Task_MoveToTarget.h"


EBTNodeResult::Type UMoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// TODO: Retrieve the vector from the BlackboardKey selected and call MoveToLocation.
    // TODO: We have to avoid to call again and again at each tick the MoveToLocation. Adding a check for the target point on the pathfollowingcomponent may work?
	return EBTNodeResult::Succeeded;
}
