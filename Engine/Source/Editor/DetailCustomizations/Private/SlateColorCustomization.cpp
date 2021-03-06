// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "DetailCustomizationsPrivatePCH.h"
#include "SlateColorCustomization.h"

TSharedRef<IPropertyTypeCustomization> FSlateColorCustomization::MakeInstance() 
{
	return MakeShareable( new FSlateColorCustomization() );
}

void FSlateColorCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> InStructPropertyHandle, class FDetailWidgetRow& InHeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	static const FName ColorUseRuleKey(TEXT("ColorUseRule"));
	static const FName SpecifiedColorKey(TEXT("SpecifiedColor"));

	StructPropertyHandle = InStructPropertyHandle;

	ColorRuleHandle = InStructPropertyHandle->GetChildHandle(ColorUseRuleKey);
	SpecifiedColorHandle = InStructPropertyHandle->GetChildHandle(SpecifiedColorKey);

	check(ColorRuleHandle.IsValid());
	check(SpecifiedColorHandle.IsValid());

	ColorRuleHandle->MarkHiddenByCustomization();
	SpecifiedColorHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(SharedThis(this), &FSlateColorCustomization::OnValueChanged));

	FColorStructCustomization::CustomizeHeader(SpecifiedColorHandle.ToSharedRef(), InHeaderRow, StructCustomizationUtils);
}

void FSlateColorCustomization::MakeHeaderRow(TSharedRef<class IPropertyHandle>& InStructPropertyHandle, FDetailWidgetRow& Row)
{
	// NOTE: Ignore InStructPropertyHandle, it's going to be the specified color handle that we passed to the color customization base class.

	Row
		.NameContent()
		[
			SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(StructPropertyHandle->GetPropertyDisplayName())
			.ToolTipText(StructPropertyHandle->GetToolTipText())
		]
		.ValueContent()
		.MinDesiredWidth(250.0f)
		.MaxDesiredWidth(250.0f)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				CreateColorWidget()
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2, 0, 0, 0)
			[
				SNew(SCheckBox)
				.IsChecked(this, &FSlateColorCustomization::GetForegroundCheckState)
				.OnCheckStateChanged(this, &FSlateColorCustomization::HandleForegroundChanged)
				[
					SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFont())
					.Text(NSLOCTEXT("SlateColorCustomization", "Inherit", "Inherit"))
					.ToolTipText(NSLOCTEXT("SlateColorCustomization", "InheritToolTip", "Uses the foreground color inherited down the widget hierarchy"))
				]
			]
		];
}

void FSlateColorCustomization::OnValueChanged()
{
	ColorRuleHandle->SetValueFromFormattedString(TEXT("UseColor_Specified"));
}

ESlateCheckBoxState::Type FSlateColorCustomization::GetForegroundCheckState() const
{
	FString ColorRuleValue;
	ColorRuleHandle->GetValueAsFormattedString(ColorRuleValue);

	if ( ColorRuleValue == TEXT("UseColor_Foreground") )
	{
		return ESlateCheckBoxState::Checked;
	}

	return ESlateCheckBoxState::Unchecked;
}

void FSlateColorCustomization::HandleForegroundChanged(ESlateCheckBoxState::Type CheckedState)
{
	if ( CheckedState == ESlateCheckBoxState::Checked )
	{
		ColorRuleHandle->SetValueFromFormattedString(TEXT("UseColor_Foreground"));
	}
	else
	{
		ColorRuleHandle->SetValueFromFormattedString(TEXT("UseColor_Specified"));
	}
}
