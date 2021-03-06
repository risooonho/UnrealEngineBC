// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "UnrealCEFSubProcess.h"

int main(int argc, char *argv[])
{
#if WITH_CEF3
	// Structure for passing command-line arguments.
	// The definition of this structure is platform-specific.
	CefMainArgs MainArgs();

	return RunCEFSubProcess(MainArgs);
#else
	return 0;
#endif
}
