#pragma once

#include "PropertyEditor/Public/IPropertyTypeCustomization.h"
#include "Setting/DialogSystemSettings.h"

class IPropertyHandle;
class FDetailWidgetRow;
class SNameComboBox;

/**
 * inspired from this https://ue4community.wiki/legacy/customizing-detail-panels-g529msrd
 * and this
 * http://kantandev.com/articles/details-panel-customization#:~:text=The%20UE4%20details%20panel%20is,and%20more%20intuitive%20for%20designers.
 * but
 * TODO: Wiki doc needs improvement, my discord question:
 * https://discordapp.com/channels/679064182297198622/679064182297198625/766620770855419915
 * All slate Widgets API doc @see https://docs.unrealengine.com/en-US/API/Runtime/Slate/Widgets/index.html
 */
class FNDialogFactorSettingsCustomization : public IPropertyTypeCustomization
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
	TSharedRef<SWidget> MakeUpButton(
		FSimpleDelegate OnClicked, TAttribute<FText> OptionalToolTipText = FText(), TAttribute<bool> IsEnabled = true);
	TSharedRef<SWidget> MakeDownButton(
		FSimpleDelegate OnClicked, TAttribute<FText> OptionalToolTipText = FText(), TAttribute<bool> IsEnabled = true);
	void RemoveButton_OnClick();
	void AddButton_OnClick();
	void UpButton_OnClick();
	void DownButton_OnClick();
	FLinearColor GetColor(TSharedPtr<IPropertyHandle> CategoryHandle) const;
	void OnCategoryChanged(TSharedPtr<IPropertyHandle> CategoryHandle);

private:
	TSharedPtr<IPropertyHandle> StructHandle;
	TArray<TSharedPtr<FName>> CategoryList;
	TMap<TSharedPtr<FName>, FNDialogResponseCategorySettings> SettingsList;
	TSharedPtr<SNameComboBox> NameComboBox;
	FLinearColor BgColor;
};
