// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "EnginePrivate.h"
#include "VisualLogger/VisualLogger.h"
#include "VisualLogger/VisualLogger.h"
#include "VisualLogger/VisualLoggerBinaryFileDevice.h"

#if ENABLE_VISUAL_LOG 

DEFINE_STAT(STAT_VisualLog);
DEFINE_LOG_CATEGORY(LogVisual);

TMap<UObject*, TArray<TWeakObjectPtr<const UObject> > > FVisualLogger::RedirectionMap;

FVisualLogger::FVisualLogger()
{
	BlockAllCategories(false);
	AddDevice(&FVisualLoggerBinaryFileDevice::Get());
	SetIsRecording(GEngine ? !!GEngine->bEnableVisualLogRecordingOnStart : false);
	SetIsRecordingOnServer(false);

	if (FParse::Param(FCommandLine::Get(), TEXT("EnableAILogging")))
	{
		SetIsRecording(true);
		SetIsRecordingToFile(true);
	}
}

void FVisualLogger::Shutdown()
{
	SetIsRecording(false);
	SetIsRecordingToFile(false);

	if (UseBinaryFileDevice)
	{
		RemoveDevice(&FVisualLoggerBinaryFileDevice::Get());
	}
}

void FVisualLogger::Cleanup(bool bReleaseMemory)
{
	const bool WasRecordingToFile = IsRecordingToFile();
	if (WasRecordingToFile)
	{
		SetIsRecordingToFile(false);
	}

	auto& OutputDevices = FVisualLogger::Get().OutputDevices;
	for (auto* Device : OutputDevices)
	{
		Device->Cleanup(bReleaseMemory);
	}

	RedirectionMap.Reset();
	LastUniqueIds.Reset();
	CurrentEntryPerObject.Reset();

	if (WasRecordingToFile)
	{
		SetIsRecordingToFile(true);
	}
}

int32 FVisualLogger::GetUniqueId(float Timestamp)
{
	return LastUniqueIds.FindOrAdd(Timestamp)++;
}

void FVisualLogger::Redirect(UObject* FromObject, UObject* ToObject)
{
	if (FromObject == ToObject)
	{
		return;
	}

	UObject* OldRedirection = FindRedirection(FromObject);
	UObject* NewRedierection = FindRedirection(ToObject);

	auto OldArray = RedirectionMap.Find(OldRedirection);
	if (OldArray)
	{
		OldArray->RemoveSingleSwap(FromObject);
	}

	RedirectionMap.FindOrAdd(NewRedierection).AddUnique(FromObject);

	UE_CVLOG(FromObject != NULL, FromObject, LogVisual, Log, TEXT("Redirected '%s' to '%s'"), *FromObject->GetName(), *NewRedierection->GetName());
}

class UObject* FVisualLogger::FindRedirection(const UObject* Object)
{
	if (RedirectionMap.Contains(Object) == false)
	{
		for (auto& Redirection : RedirectionMap)
		{
			if (Redirection.Value.Find(Object) != INDEX_NONE)
			{
				return FindRedirection(Redirection.Key);
			}
		}
	}

	return const_cast<class UObject*>(Object);
}

void FVisualLogger::SetIsRecording(bool InIsRecording) 
{ 
	if (IsRecordingToFile())
	{
		SetIsRecordingToFile(false);
	}
	bIsRecording = InIsRecording; 
};

void FVisualLogger::SetIsRecordingToFile(bool InIsRecording)
{
	if (!bIsRecording && InIsRecording)
	{
		SetIsRecording(true);
	}

	const FString BaseFileName = LogFileNameGetter.IsBound() ? LogFileNameGetter.Execute() : TEXT("VisualLog");
	const FString MapName = GWorld.GetReference() ? GWorld->GetMapName() : TEXT("");

	FString OutputFileName = FString::Printf(TEXT("%s_%s"), *BaseFileName, *MapName);

	if (bIsRecordingToFile && !InIsRecording)
	{
		for (auto* Device : OutputDevices)
		{
			if (Device->HasFlags(EVisualLoggerDeviceFlags::CanSaveToFile))
			{
				Device->SetFileName(OutputFileName);
				Device->StopRecordingToFile(GWorld.GetReference() ? GWorld->TimeSeconds : StartRecordingToFileTime);
			}
		}
	}
	else if (!bIsRecordingToFile && InIsRecording)
	{
		StartRecordingToFileTime = GWorld.GetReference() ? GWorld->TimeSeconds : 0;
		for (auto* Device : OutputDevices)
		{
			if (Device->HasFlags(EVisualLoggerDeviceFlags::CanSaveToFile))
			{
				Device->StartRecordingToFile(StartRecordingToFileTime);
			}
		}
	}

	bIsRecordingToFile = InIsRecording;
}

#endif //ENABLE_VISUAL_LOG

const FGuid EVisualLoggerVersion::GUID = FGuid(0xA4237A36, 0xCAEA41C9, 0x8FA218F8, 0x58681BF3);
FCustomVersionRegistration GVisualLoggerVersion(EVisualLoggerVersion::GUID, EVisualLoggerVersion::LatestVersion, TEXT("VisualLogger"));

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

#include "Developer/LogVisualizer/Public/LogVisualizerModule.h"

static class FLogVisualizerExec : private FSelfRegisteringExec
{
public:
	/** Console commands, see embeded usage statement **/
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override
	{
		if (FParse::Command(&Cmd, TEXT("VISLOG")))
		{
#if ENABLE_VISUAL_LOG
			FString Command = FParse::Token(Cmd, 0);
			if (Command == TEXT("record"))
			{
				FVisualLogger::Get().SetIsRecording(true);
				return true;
			}
			else if (Command == TEXT("stop"))
			{
				FVisualLogger::Get().SetIsRecording(false);
				return true;
			}
			else if (Command == TEXT("disableallbut"))
			{
				FString Category = FParse::Token(Cmd, 1);
				FVisualLogger::Get().BlockAllCategories(true);
				FVisualLogger::Get().GetWhiteList().AddUnique(*Category);
				return true;
			}
#if WITH_EDITOR
			else if (Command == TEXT("exit"))
			{
				FLogVisualizerModule::Get()->CloseUI(InWorld);
				return true;
			}
			else
			{
				FLogVisualizerModule::Get()->SummonUI(InWorld);
				return true;
			}
#endif
#else
			UE_LOG(LogVisual, Warning, TEXT("Unable to open LogVisualizer - logs are disabled"));
#endif
		}
		return false;
	}
} LogVisualizerExec;


#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
