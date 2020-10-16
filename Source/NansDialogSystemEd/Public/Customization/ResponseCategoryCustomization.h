#pragma once

#include "PropertyEditor/Public/IPropertyTypeCustomization.h"
#include "Setting/DialogSystemSettings.h"

class IPropertyHandle;
class FDetailWidgetRow;
class SNameComboBox;

class FNResponseCategoryCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
		class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
		class IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

protected:
	void OnAttributeSelected(TSharedPtr<FName> Selection, ESelectInfo::Type SelectInfo);

private:
	TSharedPtr<IPropertyHandle> NameProperty;
	TSharedPtr<IPropertyHandle> FactorNameProperty;
	TSharedPtr<IPropertyHandle> ColorProperty;
	TArray<TSharedPtr<FName>> CategoryList;
	TMap<TSharedPtr<FName>, FNDialogResponseCategorySettings> SettingsList;
	TSharedPtr<SNameComboBox> NameComboBox;
};
