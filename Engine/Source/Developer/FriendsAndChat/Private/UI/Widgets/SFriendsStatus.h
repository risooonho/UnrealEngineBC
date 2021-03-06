// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

class SFriendsStatus : public SUserWidget
{
public:

	SLATE_USER_ARGS(SFriendsStatus) { }
	SLATE_ARGUMENT(const FFriendsAndChatStyle*, FriendStyle)
	SLATE_ARGUMENT(SMenuAnchor::EMethod, Method)
	SLATE_END_ARGS()

	virtual void Construct(const FArguments& InArgs, const TSharedRef<class FFriendsStatusViewModel>& ViewModel) = 0;
};
