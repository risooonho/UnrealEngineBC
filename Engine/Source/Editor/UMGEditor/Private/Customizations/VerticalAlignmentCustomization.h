// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

class FVerticalAlignmentCustomization : public IPropertyTypeCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<class IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FVerticalAlignmentCustomization());
	}

	FVerticalAlignmentCustomization()
	{
	}
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, class IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	
	void HandleCheckStateChanged(ESlateCheckBoxState::Type InCheckboxState, TSharedRef<IPropertyHandle> PropertyHandle, EVerticalAlignment ToAlignment);

	ESlateCheckBoxState::Type GetCheckState(TSharedRef<IPropertyHandle> PropertyHandle, EVerticalAlignment ForAlignment) const;
};
