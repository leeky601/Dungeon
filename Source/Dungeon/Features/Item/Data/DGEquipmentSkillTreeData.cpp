#include "Features/Item/Data/DGEquipmentSkillTreeData.h"

FPrimaryAssetId UDGEquipmentSkillTreeData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("SkillTree"), SkillTreeID.IsValid() ? SkillTreeID.GetTagName() : GetFName());
}

bool UDGEquipmentSkillTreeData::CanSelectNode(FGameplayTag NodeID, int32 CurrentSessionLevel, const TArray<FGameplayTag>& SelectedNodeTags) const
{
	const FDGSkillTreeNode* Node = FindNode(NodeID);
	if (!Node)
	{
		return false;
	}

	if (SelectedNodeTags.Contains(NodeID))
	{
		return false;
	}

	if (CurrentSessionLevel < Node->RequiredSessionLevel)
	{
		return false;
	}

	for (const FGameplayTag& PreviousNodeID : Node->RequiredPreviousNodes)
	{
		if (!SelectedNodeTags.Contains(PreviousNodeID))
		{
			return false;
		}
	}

	return true;
}

const FDGSkillTreeNode* UDGEquipmentSkillTreeData::FindNode(FGameplayTag NodeID) const
{
	return Nodes.FindByPredicate([&NodeID](const FDGSkillTreeNode& Node)
	{
		return Node.NodeID == NodeID;
	});
}