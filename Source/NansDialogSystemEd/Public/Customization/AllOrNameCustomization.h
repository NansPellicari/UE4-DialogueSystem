// Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
class FNAllOrNameCustomization : public IPropertyTypeCustomization
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

private:
	FString OutText;

	/**
	* This method is bind to the SetOnPropertyValueChanged on the "Type" property.
	* It retrieves the Type's value and store it to the "ChosenTypeText" property here.
	*/
	void OnValuesChanged(TSharedRef<IPropertyHandle> StructPropertyHandle);
};
