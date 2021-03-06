USE [CrashReport]਍䜀伀ഀഀ
/****** Object:  StoredProcedure [dbo].[UpdateCrashesByPattern]    Script Date: 2014-08-25 15:32:17 ******/਍匀䔀吀 䄀一匀䤀开一唀䰀䰀匀 伀一ഀഀ
GO਍匀䔀吀 儀唀伀吀䔀䐀开䤀䐀䔀一吀䤀䘀䤀䔀刀 伀一ഀഀ
GO਍ഀഀ
਍䄀䰀吀䔀刀 倀刀伀䌀䔀䐀唀刀䔀 嬀搀戀漀崀⸀嬀唀瀀搀愀琀攀䌀爀愀猀栀攀猀䈀礀倀愀琀琀攀爀渀崀ഀഀ
AS਍䈀䔀䜀䤀一ഀഀ
	-- SET NOCOUNT ON added to prevent extra result sets from਍ऀⴀⴀ 椀渀琀攀爀昀攀爀椀渀最 眀椀琀栀 匀䔀䰀䔀䌀吀 猀琀愀琀攀洀攀渀琀猀⸀ഀഀ
	SET NOCOUNT ON;਍ऀഀഀ
	-- Update crash user names਍ऀ唀倀䐀䄀吀䔀 嬀搀戀漀崀⸀嬀䌀爀愀猀栀攀猀崀ഀഀ
	SET [UserName] = CASE WHEN c.[UserName] is NULL THEN u.[UserName] ELSE c.[UserName] END਍ऀ䘀刀伀䴀 嬀搀戀漀崀⸀嬀䌀爀愀猀栀攀猀崀 挀ഀഀ
	LEFT JOIN [dbo].[Users] u on (u.[id] = c.[UserNameId]);਍ऀഀഀ
	--Create Buggs਍ऀ䴀䔀刀䜀䔀 䈀甀最最猀 䄀匀 䈀ഀഀ
	USING ਍ऀ⠀ഀഀ
		SELECT [TTPID]਍ऀऀऀ  Ⰰ嬀倀愀琀琀攀爀渀崀ഀഀ
			  ,[NumberOfCrashes]਍ऀऀऀ  Ⰰ嬀一甀洀戀攀爀伀昀唀猀攀爀猀崀ഀഀ
			  ,[TimeOfFirstCrash]਍ऀऀऀ  Ⰰ嬀吀椀洀攀伀昀䰀愀猀琀䌀爀愀猀栀崀ഀഀ
			  ,[BuildVersion]਍ऀऀऀ  Ⰰ嬀匀甀洀洀愀爀礀崀ഀഀ
			  ,[CrashType]਍ऀऀऀ  Ⰰ嬀匀琀愀琀甀猀崀ഀഀ
			  ,[FixedChangeList]਍ऀऀऀ  Ⰰ嬀䜀愀洀攀崀ഀഀ
		FROM਍ऀऀ⠀ഀഀ
			SELECT COUNT(1) as NumberOfCrashes਍ऀऀऀऀ  Ⰰ 洀愀砀⠀嬀吀椀洀攀伀昀䌀爀愀猀栀崀⤀ 愀猀 吀椀洀攀伀昀䰀愀猀琀䌀爀愀猀栀ഀഀ
				  , min([TimeOfCrash]) as TimeOfFirstCrash਍ऀऀऀऀ  Ⰰ 洀愀砀⠀嬀䈀甀椀氀搀嘀攀爀猀椀漀渀崀⤀ 愀猀 䈀甀椀氀搀嘀攀爀猀椀漀渀ഀഀ
				  , max([Summary]) as Summary਍ऀऀऀऀ  Ⰰ 洀愀砀⠀嬀䌀爀愀猀栀吀礀瀀攀崀⤀ 愀猀 䌀爀愀猀栀吀礀瀀攀ഀഀ
				  , count(distinct [UserName]) as NumberOfUsers਍ऀऀऀऀ  Ⰰ 䌀漀甀渀琀⠀搀椀猀琀椀渀挀琀 䜀愀洀攀一愀洀攀⤀ 愀猀 䜀愀洀攀一愀洀攀䌀漀甀渀琀ഀഀ
				  , MIN(GameName) as Game਍ऀऀऀऀ  Ⰰ 䴀愀砀⠀嬀匀琀愀琀甀猀崀⤀ 愀猀 嬀匀琀愀琀甀猀崀ഀഀ
				  , Max(TTPID) as TTPID਍ऀऀऀऀ  Ⰰ 䴀愀砀⠀䘀椀砀攀搀䌀栀愀渀最攀䰀椀猀琀⤀ 愀猀 䘀椀砀攀搀䌀栀愀渀最攀䰀椀猀琀ഀഀ
				  , Pattern਍ऀऀऀ  䘀刀伀䴀 嬀搀戀漀崀⸀嬀䌀爀愀猀栀攀猀崀 挀 ഀഀ
			  WHERE਍ऀऀऀऀ倀愀琀琀攀爀渀 椀猀 渀漀琀 一唀䰀䰀 䄀一䐀 倀愀琀琀攀爀渀 渀漀琀 氀椀欀攀 ✀✀ഀഀ
			  group by Pattern਍ऀऀ  ⤀ 愀猀 猀ഀഀ
		 Where s.NumberOfCrashes > 1਍ऀ⤀ 䄀匀 䌀 ഀഀ
	ON (B.Pattern = C.Pattern)਍ऀ圀䠀䔀一 一伀吀 䴀䄀吀䌀䠀䔀䐀 䈀夀 吀䄀刀䜀䔀吀ഀഀ
		THEN INSERT (਍ऀऀऀऀ嬀吀吀倀䤀䐀崀ഀഀ
			  ,[Pattern]਍ऀऀऀ  Ⰰ嬀一甀洀戀攀爀伀昀䌀爀愀猀栀攀猀崀ഀഀ
			  ,[NumberOfUsers]਍ऀऀऀ  Ⰰ嬀吀椀洀攀伀昀䘀椀爀猀琀䌀爀愀猀栀崀ഀഀ
			  ,[TimeOfLastCrash]਍ऀऀऀ  Ⰰ嬀䈀甀椀氀搀嘀攀爀猀椀漀渀崀ഀഀ
			  ,[SummaryV2] -- This a new summary਍ऀऀऀ  Ⰰ嬀䌀爀愀猀栀吀礀瀀攀崀ഀഀ
			  ,[Status]਍ऀऀऀ  Ⰰ嬀䘀椀砀攀搀䌀栀愀渀最攀䰀椀猀琀崀ഀഀ
			  ,[Game]਍ऀऀऀ  ⤀ ഀഀ
		Values(਍ऀऀऀऀ䌀⸀嬀吀吀倀䤀䐀崀ഀഀ
			  , C.[Pattern]਍ऀऀऀ  Ⰰ 䌀⸀嬀一甀洀戀攀爀伀昀䌀爀愀猀栀攀猀崀ഀഀ
			  , C.[NumberOfUsers]਍ऀऀऀ  Ⰰ 䌀⸀嬀吀椀洀攀伀昀䘀椀爀猀琀䌀爀愀猀栀崀ഀഀ
			  , C.[TimeOfLastCrash]਍ऀऀऀ  Ⰰ 䌀⸀嬀䈀甀椀氀搀嘀攀爀猀椀漀渀崀ഀഀ
			  , C.[Summary]਍ऀऀऀ  Ⰰ 䌀⸀嬀䌀爀愀猀栀吀礀瀀攀崀ഀഀ
			  , C.[Status]਍ऀऀऀ  Ⰰ 䌀⸀嬀䘀椀砀攀搀䌀栀愀渀最攀䰀椀猀琀崀ഀഀ
			  , C.[Game]਍ऀऀऀ  ⤀ ഀഀ
਍ऀ圀䠀䔀一 䴀䄀吀䌀䠀䔀䐀 ഀഀ
		THEN UPDATE SET B.[NumberOfCrashes] = C.[NumberOfCrashes]਍ऀऀऀऀऀऀⰀ 䈀⸀嬀吀椀洀攀伀昀䰀愀猀琀䌀爀愀猀栀崀 㴀 䌀⸀嬀吀椀洀攀伀昀䰀愀猀琀䌀爀愀猀栀崀ഀഀ
						, B.[BuildVersion] = C.[BuildVersion]਍ऀऀऀऀऀऀⰀ 䈀⸀嬀匀甀洀洀愀爀礀嘀㈀崀 㴀 䌀⸀嬀匀甀洀洀愀爀礀崀 ⴀⴀ 吀栀椀猀 愀 渀攀眀 猀甀洀洀愀爀礀ഀഀ
						, B.[CrashType] = C.[CrashType]਍ऀऀऀऀऀऀⰀ 䈀⸀嬀一甀洀戀攀爀伀昀唀猀攀爀猀崀 㴀 䌀⸀嬀一甀洀戀攀爀伀昀唀猀攀爀猀崀ഀഀ
਍ऀ伀唀吀倀唀吀 ␀愀挀琀椀漀渀Ⰰ 䤀渀猀攀爀琀攀搀⸀⨀Ⰰ 䐀攀氀攀琀攀搀⸀⨀㬀ഀഀ
		਍ऀऀഀഀ
		਍ऀ⼀⨀⨀⨀⨀⨀⨀ 䨀漀椀渀 䈀甀最最猀 愀渀搀 䌀爀愀猀栀攀猀  ⨀⨀⨀⨀⨀⨀⼀ഀഀ
	MERGE dbo.Buggs_Crashes BC ਍ऀ唀匀䤀一䜀 ഀഀ
	( ਍ऀऀ匀䔀䰀䔀䌀吀  戀⸀䤀搀 愀猀 䈀甀最最䤀搀Ⰰ 挀⸀嬀䤀搀崀 愀猀 䌀爀愀猀栀䤀搀ഀഀ
		FROM [dbo].[Crashes] c਍ऀऀ䨀漀椀渀 嬀搀戀漀崀⸀䈀甀最最猀 戀 漀渀 ⠀戀⸀倀愀琀琀攀爀渀 㴀 挀⸀倀愀琀琀攀爀渀⤀ഀഀ
		group by b.Id, c.Id਍ऀ ⤀ 䄀匀 䌀ഀഀ
	 ON BC.BuggId = C.BuggId AND BC.CrashId = C.CrashId਍ऀ 圀䠀䔀一 一伀吀 䴀䄀吀䌀䠀䔀䐀 䈀夀 吀䄀刀䜀䔀吀ഀഀ
		THEN INSERT ਍ऀऀऀ⠀䈀甀最最䤀搀Ⰰ 䌀爀愀猀栀䤀搀⤀ഀഀ
		VALUES ਍ऀऀऀ⠀䌀⸀䈀甀最最䤀搀Ⰰ 䌀⸀䌀爀愀猀栀䤀搀⤀ऀഀഀ
	 WHEN MATCHED਍ऀऀ吀䠀䔀一 唀倀䐀䄀吀䔀 匀䔀吀ഀഀ
			BC.BuggId = C.BuggId, ਍ऀऀऀ䈀䌀⸀䌀爀愀猀栀䤀搀 㴀 䌀⸀䌀爀愀猀栀䤀搀ഀഀ
	OUTPUT $action, Inserted.*, Deleted.*;਍ऀഀഀ
	਍ऀ⼀⨀⨀⨀⨀⨀⨀ 䨀漀椀渀 䈀甀最最猀开唀猀攀爀猀 愀渀搀 䌀爀愀猀栀攀猀  ⨀⨀⨀⨀⨀⨀⼀ഀഀ
	MERGE dbo.Buggs_Users AS BU਍ऀ唀匀䤀一䜀ഀഀ
		(਍ऀऀ  匀䔀䰀䔀䌀吀  戀⸀䤀搀 愀猀 䈀甀最最䤀搀Ⰰ 挀⸀嬀唀猀攀爀一愀洀攀崀 愀猀 唀猀攀爀一愀洀攀ഀഀ
		  FROM [dbo].[Crashes] c਍ऀऀ  䨀漀椀渀 嬀搀戀漀崀⸀䈀甀最最猀 戀 漀渀 ⠀戀⸀倀愀琀琀攀爀渀 㴀 挀⸀倀愀琀琀攀爀渀⤀ഀഀ
		  Group by b.Id, c.UserName਍ऀऀ⤀ 䄀匀 䌀ഀഀ
	ON BU.BuggId = C.BuggId AND BU.UserName = C.UserName਍ऀ 圀䠀䔀一 一伀吀 䴀䄀吀䌀䠀䔀䐀 䈀夀 吀䄀刀䜀䔀吀ഀഀ
		THEN INSERT ਍ऀऀऀ⠀䈀甀最最䤀搀Ⰰ 唀猀攀爀一愀洀攀⤀ ഀഀ
		VALUES ਍ऀऀऀ⠀䌀⸀䈀甀最最䤀搀Ⰰ 䌀⸀唀猀攀爀一愀洀攀⤀ऀഀഀ
	 WHEN MATCHED਍ऀऀ吀䠀䔀一 唀倀䐀䄀吀䔀 匀䔀吀ഀഀ
			BU.BuggId = C.BuggId, ਍ऀऀऀ䈀唀⸀唀猀攀爀一愀洀攀 㴀 䌀⸀唀猀攀爀一愀洀攀ഀഀ
	OUTPUT $action, Inserted.*, Deleted.*;਍ऀഀഀ
		  ਍ऀऀഀഀ
	  ਍ऀ⼀⨀⨀⨀⨀⨀⨀ 䨀漀椀渀 䈀甀最最猀 愀渀搀 唀猀攀爀䜀爀漀甀瀀猀  ⨀⨀⨀⨀⨀⨀⼀ഀഀ
	MERGE dbo.Buggs_UserGroups AS BUG਍ऀ唀匀䤀一䜀ഀഀ
	(਍ऀ  匀攀氀攀挀琀 䈀甀最最䤀搀Ⰰ 唀猀攀爀䜀爀漀甀瀀䤀搀ഀഀ
	  From [dbo].[Buggs_Users] bu਍ऀ  䨀漀椀渀 搀戀漀⸀唀猀攀爀猀 甀 漀渀 ⠀甀⸀唀猀攀爀一愀洀攀 㴀 戀甀⸀唀猀攀爀一愀洀攀⤀ഀഀ
	  Group by BuggId, UserGroupId਍ऀ ⤀ 䄀匀 䈀唀ഀഀ
	 ON  BUG.BuggId = BU.BuggId AND BUG.UserGroupId = BU.UserGroupId਍ऀ 圀䠀䔀一 一伀吀 䴀䄀吀䌀䠀䔀䐀 䈀夀 吀䄀刀䜀䔀吀ഀഀ
		THEN INSERT ਍ऀऀऀ⠀䈀甀最最䤀搀Ⰰ 唀猀攀爀䜀爀漀甀瀀䤀搀⤀ ഀഀ
		VALUES ਍ऀऀऀ⠀䈀唀⸀䈀甀最最䤀搀Ⰰ 䈀唀⸀唀猀攀爀䜀爀漀甀瀀䤀搀⤀ऀഀഀ
	 WHEN MATCHED਍ऀऀ吀䠀䔀一 唀倀䐀䄀吀䔀 匀䔀吀ഀഀ
			BUG.BuggId = BU.BuggId, ਍ऀऀऀ䈀唀䜀⸀唀猀攀爀䜀爀漀甀瀀䤀搀 㴀 䈀唀⸀唀猀攀爀䜀爀漀甀瀀䤀搀ഀഀ
	OUTPUT $action, Inserted.*, Deleted.*;਍ऀ ഀഀ
	/*** Update Crashes to match the Bugg Status, TTPID, and Fixed Change List ****/਍ऀⴀⴀ 吀栀椀猀 栀愀渀搀氀攀猀 渀攀眀 挀爀愀猀栀攀猀 琀栀愀琀 攀渀琀攀爀 琀栀攀 猀礀猀琀攀洀㬀 椀琀 栀愀猀 琀栀攀 猀椀搀攀 攀昀昀攀挀琀 漀昀 洀愀欀椀渀最 琀栀攀 䈀甀最最 愀甀琀栀漀爀椀琀愀琀椀瘀攀 椀渀 琀栀椀猀 挀愀猀攀 戀甀琀 琀栀愀琀✀猀 栀漀眀 琀栀攀礀 猀栀漀甀氀搀 戀攀 甀猀攀搀⸀ഀഀ
	update C਍ऀ匀䔀吀 ഀഀ
		C.TTPID = B.TTPID,਍ऀऀ䌀⸀䘀椀砀攀搀䌀栀愀渀最攀䰀椀猀琀 㴀 䈀⸀䘀椀砀攀搀䌀栀愀渀最攀䰀椀猀琀Ⰰഀഀ
		C.[Status] = B.[Status]਍ऀ䘀爀漀洀 ഀഀ
		dbo.Crashes as C਍ऀऀ䨀漀椀渀 䈀甀最最猀开䌀爀愀猀栀攀猀 愀猀 䈀䌀 伀一 ⠀挀⸀䤀搀 㴀 䈀䌀⸀䌀爀愀猀栀䤀搀⤀ഀഀ
		Join Buggs as B ON (B.Id =BC.BuggId AND C.Id = BC.CrashId) ਍ऀ ഀഀ
END਍ഀഀ
਍�