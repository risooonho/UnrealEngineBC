// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.

#include "AbilitySystemEditorPrivatePCH.h"
#include "AttributeDetails.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"
#include "KismetEditorUtilities.h"
#include "AttributeSet.h"
#include "GameplayAbilitiesModule.h"
#include "SGameplayAttributeGraphPin.h"
#include "SSearchBox.h"
#include "STextComboBox.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

#define LOCTEXT_NAMESPACE "AttributeDetailsCustomization"

DEFINE_LOG_CATEGORY(LogAttributeDetails);

TSharedRef<IPropertyTypeCustomization> FAttributePropertyDetails::MakeInstance()
{
	return MakeShareable(new FAttributePropertyDetails());
}

void FAttributePropertyDetails::CustomizeHeader( TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{
	MyProperty = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGameplayAttribute,Attribute));

	PropertyOptions.Empty();
	PropertyOptions.Add(MakeShareable(new FString("None")));

	FString FilterMetaStr = StructPropertyHandle->GetProperty()->GetMetaData(TEXT("FilterMetaTag"));

	TArray<UProperty*> PropertiesToAdd;


	
/*
	const bool bIsExtensionAttribute = (StructPropertyHandle->GetProperty()->GetOuter() == FExtensionAttributeModifierInfo::StaticStruct());
	if (bIsExtensionAttribute)
	{
		// Attributes in FExtensionAttributeModifierInfo in FGameplayModifierInfos can only be set to the attributes in the extension class
		TSharedPtr<IPropertyHandle> ExtensionAttributeModifierInfoHandle = StructPropertyHandle->GetParentHandle();
		TSharedPtr<IPropertyHandle> AttributeModifierArrayHandle = ExtensionAttributeModifierInfoHandle.IsValid() ? ExtensionAttributeModifierInfoHandle->GetParentHandle() : nullptr;
		TSharedPtr<IPropertyHandle> ModifierCallbackHandle = AttributeModifierArrayHandle.IsValid() ? AttributeModifierArrayHandle->GetParentHandle() : nullptr;
		if ( ModifierCallbackHandle.IsValid() )
		{
			TArray<const void*> RawPtrs;
			ModifierCallbackHandle->AccessRawData(RawPtrs);
			if ( RawPtrs.Num() > 0 )
			{
				// Only allow setting of attributes of all selected structs have the same extension class
				const FGameplayModifierCallback& FirstCallback = *reinterpret_cast<const FGameplayModifierCallback*>(RawPtrs[0]);
				TSubclassOf<UGameplayEffectExtension> CommonExtensionClass = FirstCallback.ExtensionClass;
				bool bHasCommonExtensionClass = true;

				for( const void* Ptr : RawPtrs )
				{
					const FGameplayModifierCallback& Callback = *reinterpret_cast<const FGameplayModifierCallback*>(Ptr);
					if ( Callback.ExtensionClass != CommonExtensionClass )
					{
						bHasCommonExtensionClass = false;
						break;
					}
				}

				if ( bHasCommonExtensionClass && CommonExtensionClass != nullptr )
				{
					UGameplayEffectExtension* ExtensionCDO = CommonExtensionClass->GetDefaultObject<UGameplayEffectExtension>();
					if ( ensure(ExtensionCDO) )
					{
						const bool bIsSourceAttributeModifiers = (AttributeModifierArrayHandle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED(FGameplayModifierCallback, SourceAttributeModifiers));
						if ( bIsSourceAttributeModifiers )
						{
							for (const FGameplayAttribute& RelevantAttribute : ExtensionCDO->RelevantSourceAttributes)
							{
								PropertiesToAdd.Add(RelevantAttribute.GetUProperty());
							}
						}
						else
						{
							for (const FGameplayAttribute& RelevantAttribute : ExtensionCDO->RelevantTargetAttributes)
							{
								PropertiesToAdd.Add(RelevantAttribute.GetUProperty());
							}
						}
					}
				}
			}
		}
	}
	else
*/
	{
		// Gather all UAttriubute classes
		for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
		{
			UClass *Class = *ClassIt;
			if (Class->IsChildOf(UAttributeSet::StaticClass()) && !FKismetEditorUtilities::IsClassABlueprintSkeleton(Class))
			{
				// Allow entire classes to be filtered globalyl
				if (Class->HasMetaData(TEXT("HideInDetailsView")))
				{
					continue;
				}

				for (TFieldIterator<UProperty> PropertyIt(Class, EFieldIteratorFlags::ExcludeSuper); PropertyIt; ++PropertyIt)
				{
					UProperty *Property = *PropertyIt;

					if (!FilterMetaStr.IsEmpty() && Property->HasMetaData(*FilterMetaStr))
					{
						continue;
					}

					// Allow properties to be filtered globally (never show up)
					if (Property->HasMetaData(TEXT("HideInDetailsView")))
					{
						continue;
					}
				
					PropertiesToAdd.Add(Property);
				}
			}
		}
	}

	for ( auto* Property : PropertiesToAdd )
	{
		PropertyOptions.Add(MakeShareable(new FString(FString::Printf(TEXT("%s.%s"), *Property->GetOuter()->GetName(), *Property->GetName()))));
	}

	// Fixme: this should be unified to use SGameplayATtributeWidget instead of custom combo box

	HeaderRow.
		NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(500)
		.MaxDesiredWidth(4096)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			//.FillWidth(1.0f)
			.HAlign(HAlign_Fill)
			.Padding(0.f, 0.f, 2.f, 0.f)
			[

				SNew(STextComboBox)
				.ContentPadding(FMargin(2.0f, 2.0f))
				.OptionsSource( &PropertyOptions )
				.InitiallySelectedItem(GetPropertyType())
				.OnSelectionChanged( this, &FAttributePropertyDetails::OnChangeProperty )
			]
		];
}

void FAttributePropertyDetails::CustomizeChildren( TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{


}

TSharedPtr<FString> FAttributePropertyDetails::GetPropertyType() const
{
	if (MyProperty.IsValid())
	{
		UProperty *PropertyValue = NULL;
		UObject *ObjPtr = NULL;
		MyProperty->GetValue(ObjPtr);
		PropertyValue = Cast<UProperty>(ObjPtr);
		if (PropertyValue)
		{
			FString FullString = PropertyValue->GetOuter()->GetName() + TEXT(".") + PropertyValue->GetName();
			for (int32 i=0; i < PropertyOptions.Num(); ++i)
			{
				if (PropertyOptions[i].IsValid() && PropertyOptions[i].Get()->Equals(FullString))
				{
					return PropertyOptions[i];
				}
			}
		}
	}

	return PropertyOptions[0]; // This should always be none
}

void FAttributePropertyDetails::OnChangeProperty(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
	if (ItemSelected.IsValid() && MyProperty.IsValid())
	{		
		FString FullString = *ItemSelected.Get();
		FString ClassName;
		FString PropertyName;

		FullString.Split( TEXT("."), &ClassName, &PropertyName);

		UClass *FoundClass = FindObject<UClass>(ANY_PACKAGE, *ClassName);
		if (FoundClass)
		{
			UProperty *Property = FindField<UProperty>(FoundClass, *PropertyName);
			if (Property)
			{
				const UObject* ObjPtr = Property;
				MyProperty->SetValue(ObjPtr);
				
				return;
			}
		}

		const UObject* nullObj = nullptr;
		MyProperty->SetValue(nullObj);
	}

	
}

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

TSharedRef<IDetailCustomization> FAttributeDetails::MakeInstance()
{
	return MakeShareable(new FAttributeDetails);
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FAttributeDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	MyProperty = DetailLayout.GetProperty("PropertyReference", UAttributeSet::StaticClass());

	PropertyOptions.Empty();
	PropertyOptions.Add(MakeShareable(new FString("None")));

	for( TFieldIterator<UProperty> It(UAttributeSet::StaticClass(), EFieldIteratorFlags::ExcludeSuper) ; It ; ++It )
	{
		UProperty *Property = *It;
		PropertyOptions.Add(MakeShareable(new FString(Property->GetName())));
	}

	IDetailCategoryBuilder& Category = DetailLayout.EditCategory("Variable", LOCTEXT("VariableDetailsCategory", "Variable").ToString());
	const FSlateFontInfo DetailFontInfo = IDetailLayoutBuilder::GetDetailFont();
	
	Category.AddCustomRow( TEXT("Replication") )
		//.Visibility(TAttribute<EVisibility>(this, &FBlueprintVarActionDetails::ReplicationVisibility))
		.NameContent()
		[
			SNew(STextBlock)
			.ToolTipText(LOCTEXT("PropertyType_Tooltip", "Which Property To Modify?"))
			.Text( LOCTEXT("PropertyModifierInfo", "Property") )
			.Font( DetailFontInfo )
		]
		.ValueContent()
		[
			SNew(STextComboBox)
			.OptionsSource( &PropertyOptions )
			.InitiallySelectedItem(GetPropertyType())
			.OnSelectionChanged( this, &FAttributeDetails::OnChangeProperty )
		];
	
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


TSharedPtr<FString> FAttributeDetails::GetPropertyType() const
{
	if (!MyProperty.IsValid())
		return PropertyOptions[0];

	UProperty *PropertyValue = NULL;
	UObject *ObjPtr = NULL;
	MyProperty->GetValue(ObjPtr);
	PropertyValue = Cast<UProperty>(ObjPtr);

	if (PropertyValue != NULL)
	{
		for (int32 i=0; i < PropertyOptions.Num(); ++i)
		{
			if (PropertyOptions[i].IsValid() && PropertyOptions[i].Get()->Equals(PropertyValue->GetName()))
			{
				return PropertyOptions[i];
			}
		}
	}

	return PropertyOptions[0]; // This should always be none
}

void FAttributeDetails::OnChangeProperty(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
	if (!ItemSelected.IsValid())
	{
		return;
	}

	FString PropertyName = *ItemSelected.Get();

	for( TFieldIterator<UProperty> It(UAttributeSet::StaticClass(), EFieldIteratorFlags::ExcludeSuper) ; It ; ++It )
	{
		const UProperty *Property = *It;
		if (PropertyName == Property->GetName())
		{
			const UObject *ObjPtr = Property;
			MyProperty->SetValue(ObjPtr);
			return;
		}
	}
}

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

TSharedRef<IPropertyTypeCustomization> FScalableFloatDetails::MakeInstance()
{
	return MakeShareable(new FScalableFloatDetails());
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FScalableFloatDetails::CustomizeHeader( TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{
	uint32 NumChildren = 0;
	StructPropertyHandle->GetNumChildren(NumChildren);

	for (uint32 i = 0; i < NumChildren; i++)
	{
		FString PropName = StructPropertyHandle->GetChildHandle(i)->GetPropertyDisplayName();
		PropName;
	}

	ValueProperty = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FScalableFloat,Value));
	CurveTableHandleProperty = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FScalableFloat,Curve));

	RowNameProperty = CurveTableHandleProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurveTableRowHandle, RowName));
	CurveTableProperty = CurveTableHandleProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCurveTableRowHandle, CurveTable));

	CurrentSelectedItem = InitWidgetContent();

	FSimpleDelegate OnCurveTableChangedDelegate = FSimpleDelegate::CreateSP(this, &FScalableFloatDetails::OnCurveTableChanged);
	CurveTableProperty->SetOnPropertyValueChanged(OnCurveTableChangedDelegate);

	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth( 600 )
		.MaxDesiredWidth( 4096 )
		[
			SNew(SHorizontalBox)
			.IsEnabled(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this, &FScalableFloatDetails::IsEditable)))
			
			+SHorizontalBox::Slot()
			.FillWidth(0.12f)
			.HAlign(HAlign_Fill)
			.Padding(0.f, 0.f, 2.f, 0.f)
			[
				ValueProperty->CreatePropertyValueWidget()
			]
		
			+SHorizontalBox::Slot()
			.FillWidth(0.40f)
			.HAlign(HAlign_Fill)
			.Padding(2.f, 0.f, 2.f, 0.f)
			[
				CurveTableProperty->CreatePropertyValueWidget()
			]

			+SHorizontalBox::Slot()
			.FillWidth(0.23f)
			.HAlign(HAlign_Fill)
			.Padding(2.f, 0.f, 2.f, 0.f)
			[
				SAssignNew(RowNameComboButton, SComboButton)
				.OnGetMenuContent(this, &FScalableFloatDetails::GetListContent)
				.ContentPadding(FMargin(2.0f, 2.0f))
				.Visibility(this, &FScalableFloatDetails::GetRowNameVisibility)
				.ButtonContent()
				[
					SNew(STextBlock)
					.Text(this, &FScalableFloatDetails::GetRowNameComboBoxContentText)
				]
			]

			+SHorizontalBox::Slot()
			.FillWidth(0.15f)
			.HAlign(HAlign_Fill)
			.Padding(2.f, 0.f, 2.f, 0.f)
			[
				SNew(SVerticalBox)
				.Visibility(this, &FScalableFloatDetails::GetPreviewVisibility)
				
				+SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(this, &FScalableFloatDetails::GetRowValuePreviewLabel)
				]

				+SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(this, &FScalableFloatDetails::GetRowValuePreviewText)
				]
			]

			+SHorizontalBox::Slot()
			.FillWidth(0.1f)
			.HAlign(HAlign_Fill)
			.Padding(2.f, 0.f, 0.f, 0.f)
			[
				SNew(SSlider)
				.Visibility(this, &FScalableFloatDetails::GetPreviewVisibility)
				.ToolTipText(LOCTEXT("LevelPreviewToolTip", "Adjust the preview level."))
				.Value(this, &FScalableFloatDetails::GetPreviewLevel)
				.OnValueChanged(this, &FScalableFloatDetails::SetPreviewLevel)
			]
		];	
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION


void FScalableFloatDetails::OnCurveTableChanged()
{
	CurrentSelectedItem = InitWidgetContent();
	if (RowNameComboListView.IsValid())
	{
		RowNameComboListView->SetSelection(CurrentSelectedItem);
		RowNameComboListView->RequestListRefresh();
	}

	// Set the default value to 1.0 when using a data table, so the value in the table is used directly. Only do this if the value is currently 0 (default)
	// Set it back to 0 when setting back. Only do this if the value is currently 1 to go back to the default.
	{
		UObject* CurveTable = nullptr;
		CurveTableProperty->GetValue(CurveTable);

		float Value;
		ValueProperty->GetValue(Value);

		if ( CurveTable )
		{
			if ( Value == 0.f )
			{
				ValueProperty->SetValue(1.f);
			}
		}
		else
		{
			if ( Value == 1.f )
			{
				ValueProperty->SetValue(0.f);
			}
		}
	}
}

TSharedPtr<FString> FScalableFloatDetails::InitWidgetContent()
{
	TSharedPtr<FString> InitialValue = MakeShareable(new FString(TEXT("None")));

	FName RowName;
	const FPropertyAccess::Result RowResult = RowNameProperty->GetValue(RowName);
	RowNames.Empty();
	RowNames.Add(InitialValue);

	/** Get the properties we wish to work with */
	UCurveTable* CurveTable = GetCurveTable();

	if (CurveTable != NULL)
	{
		/** Extract all the row names from the RowMap */
		for (TMap<FName, FRichCurve*>::TConstIterator Iterator(CurveTable->RowMap); Iterator; ++Iterator)
		{
			/** Create a simple array of the row names */
			TSharedRef<FString> RowNameItem = MakeShareable(new FString(Iterator.Key().ToString()));
			RowNames.Add(RowNameItem);

			/** Set the initial value to the currently selected item */
			if (Iterator.Key() == RowName)
			{
				InitialValue = RowNameItem;
			}
		}
	}

	/** Reset the initial value to ensure a valid entry is set */
	if (RowResult != FPropertyAccess::MultipleValues)
	{
		FName NewValue = FName(**InitialValue);
		RowNameProperty->SetValue(NewValue);
	}

	return InitialValue;
}

UCurveTable * FScalableFloatDetails::GetCurveTable()
{
	UCurveTable* CurveTable = NULL;
	CurveTableProperty->GetValue((UObject*&)CurveTable);

	if (CurveTable == NULL)
	{
		CurveTable = IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals()->GetGlobalCurveTable();
	}

	return CurveTable;
}

EVisibility FScalableFloatDetails::GetRowNameVisibility() const
{
	UObject* CurveTable = nullptr;
	CurveTableProperty->GetValue(CurveTable);

	return CurveTable ? EVisibility::Visible : EVisibility::Hidden;
}

EVisibility FScalableFloatDetails::GetPreviewVisibility() const
{
	const bool bRowNameVisible = (GetRowNameVisibility() == EVisibility::Visible);
	const bool bRowNameValid = CurrentSelectedItem.IsValid() && !CurrentSelectedItem->IsEmpty() && *CurrentSelectedItem.Get() != FName(NAME_None).ToString();
	return (bRowNameVisible && bRowNameValid) ? EVisibility::Visible : EVisibility::Hidden;
}

float FScalableFloatDetails::GetPreviewLevel() const
{
	return (MaxPreviewLevel != 0) ? PreviewLevel / MaxPreviewLevel : 0;
}

void FScalableFloatDetails::SetPreviewLevel(float NewLevel)
{
	PreviewLevel = FMath::FloorToInt(NewLevel * MaxPreviewLevel);
}

TSharedRef<SWidget> FScalableFloatDetails::GetListContent()
{
	SAssignNew(RowNameComboListView, SListView<TSharedPtr<FString> >)
		.ListItemsSource(&RowNames)
		.OnSelectionChanged(this, &FScalableFloatDetails::OnSelectionChanged)
		.OnGenerateRow(this, &FScalableFloatDetails::HandleRowNameComboBoxGenarateWidget)
		.SelectionMode(ESelectionMode::Single);

	if (CurrentSelectedItem.IsValid())
	{
		RowNameComboListView->SetSelection(CurrentSelectedItem);
	}

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSearchBox)
			.OnTextChanged(this, &FScalableFloatDetails::OnFilterTextChanged)
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			RowNameComboListView.ToSharedRef()
		];
}

void FScalableFloatDetails::OnSelectionChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid())
	{
		CurrentSelectedItem = SelectedItem;
		FName NewValue = FName(**SelectedItem);
		RowNameProperty->SetValue(NewValue);

		// Close the combo
		RowNameComboButton->SetIsOpen(false);
	}
}

TSharedRef<ITableRow> FScalableFloatDetails::HandleRowNameComboBoxGenarateWidget(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
		SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(STextBlock).Text(*InItem)
		];
}

/** Display the current selection */
FString FScalableFloatDetails::GetRowNameComboBoxContentText() const
{
	FString RowName = TEXT("Multiple Values");
	const FPropertyAccess::Result RowResult = RowNameProperty->GetValue(RowName);
	if (RowResult != FPropertyAccess::MultipleValues)
	{
		TSharedPtr<FString> SelectedRowName = CurrentSelectedItem;
		if (SelectedRowName.IsValid())
		{
			RowName = *SelectedRowName;
		}
		else
		{
			RowName = TEXT("None");
		}
	}
	return RowName;
}

FText FScalableFloatDetails::GetRowValuePreviewLabel() const
{
	return FText::Format(LOCTEXT("LevelPreviewLabel", "Preview At {0}"), FText::AsNumber(PreviewLevel));
}

FString FScalableFloatDetails::GetRowValuePreviewText() const
{
	TArray<const void*> RawPtrs;
	CurveTableHandleProperty->AccessRawData(RawPtrs);
	if ( RawPtrs.Num() == 1 )
	{
		const FCurveTableRowHandle& Curve = *reinterpret_cast<const FCurveTableRowHandle*>(RawPtrs[0]);
		if ( Curve.CurveTable && Curve.RowName != NAME_None )
		{
			float Value;
			ValueProperty->GetValue(Value);

			return FString::Printf(TEXT("%.3f"), Value * Curve.Eval(PreviewLevel));
		}
	}

	return FString();
}

/** Called by Slate when the filter box changes text. */
void FScalableFloatDetails::OnFilterTextChanged(const FText& InFilterText)
{
	FString CurrentFilterText = InFilterText.ToString();

	FName RowName;
	const FPropertyAccess::Result RowResult = RowNameProperty->GetValue(RowName);
	RowNames.Empty();

	/** Get the properties we wish to work with */
	UCurveTable* CurveTable = GetCurveTable();

	if (CurveTable != NULL)
	{
		/** Extract all the row names from the RowMap */
		for (TMap<FName, FRichCurve*>::TConstIterator Iterator(CurveTable->RowMap); Iterator; ++Iterator)
		{
			/** Create a simple array of the row names */
			FString RowString = Iterator.Key().ToString();
			if (CurrentFilterText == TEXT("") || RowString.Contains(CurrentFilterText))
			{
				TSharedRef<FString> RowNameItem = MakeShareable(new FString(RowString));
				RowNames.Add(RowNameItem);
			}
		}
	}
	RowNameComboListView->RequestListRefresh();
}

bool FScalableFloatDetails::IsEditable( ) const
{
	return true;
}

void FScalableFloatDetails::CustomizeChildren( TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{
	
}

//-------------------------------------------------------------------------------------


#undef LOCTEXT_NAMESPACE
