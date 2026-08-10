#include "Features/Item/Data/DGWeaponData.h"

#include "Features/Item/Data/DGEquipmentSkillTreeData.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

bool UDGWeaponData::IsSkillTreeCompatible(const UDGEquipmentSkillTreeData* InSkillTreeData) const
{
	if (!InSkillTreeData)
	{
		return false;
	}

	return InSkillTreeData->AssociatedWeaponType == EDGWeaponType::None
		|| InSkillTreeData->AssociatedWeaponType == WeaponType;
}

#if WITH_EDITOR
EDataValidationResult UDGWeaponData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!SkillTreeData.IsNull())
	{
		const UDGEquipmentSkillTreeData* LoadedSkillTreeData = SkillTreeData.LoadSynchronous();
		if (!IsSkillTreeCompatible(LoadedSkillTreeData))
		{
			Context.AddError(FText::Format(
				NSLOCTEXT("DGWeaponData", "SkillTreeWeaponTypeMismatch", "SkillTreeData '{0}' AssociatedWeaponType does not match this weapon's WeaponType."),
				FText::FromString(GetNameSafe(LoadedSkillTreeData))));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif