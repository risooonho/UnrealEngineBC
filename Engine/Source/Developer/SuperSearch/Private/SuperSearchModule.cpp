// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "SuperSearchPrivatePCH.h"
#include "SSuperSearch.h"

IMPLEMENT_MODULE( FSuperSearchModule,SuperSearch );

namespace SuperSearchModule
{
	static const FName FNameSuperSearchApp = FName(TEXT("SuperSearchApp"));
}

void FSuperSearchModule::StartupModule()
{
}

void FSuperSearchModule::ShutdownModule()
{
}

TSharedRef< SWidget > FSuperSearchModule::MakeSearchBox( TSharedPtr< SEditableTextBox >& OutExposedEditableTextBox ) const
{
	TSharedRef< SSuperSearchBox > NewSearchBox = SNew( SSuperSearchBox );
	OutExposedEditableTextBox = NewSearchBox->GetEditableTextBox();
	return NewSearchBox;
}
