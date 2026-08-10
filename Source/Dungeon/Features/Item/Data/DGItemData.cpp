#include "Features/Item/Data/DGItemData.h"

FPrimaryAssetId UDGItemData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Item"), ItemID.IsValid() ? ItemID.GetTagName() : GetFName());
}