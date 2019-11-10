#pragma once

NTSTATUS NTAPI WnfCallback(ULONG64, void*, void*, void*, void*, ULONG);

const ULONG64 WNF_SHEL_NOTIFICATIONS = 0xD83063EA3BC1035;
const ULONG64 WNF_TMCN_ISTABLETMODE = 0xf850339a3bc0835;
const ULONG64 WNF_LFS_STATE = 0x41950821A3BC0875;
const ULONG64 WNF_SHEL_QUIETHOURS_ACTIVE_PROFILE_CHANGED = 0xD83063EA3BF1C75;
const ULONG64 WNF_SHEL_TOAST_PUBLISHED = 0xD83063EA3BD0035;
const ULONG64 WNF_VPN_CLIENT_CONNECTIVITY_STATUS = 0x41881E3BA3BC0875;
const ULONG64 WNF_WIFI_CONNECTION_SCORE = 0x880073AA3BC5875;
const ULONG64 WNF_WIFI_AVERAGE_TRANSMIT = 0x880073AA3BC6875;
const ULONG64 WNF_CNET_WIFI_ACTIVITY = 0x1583002EA3BC8075;
const ULONG64 WNF_CELL_SIGNAL_STRENGTH_BARS_CAN0 = 0xD8A0B2EA3BC1075;
const ULONG64 WNF_CELL_SIGNAL_STRENGTH_BARS_CAN1 = 0xD8A0B2EA3BD1075;
const ULONG64 WNF_IMSN_LAUNCHERVISIBILITY = 0xF950324A3BC1035;

extern "C" {

	typedef struct _WNF_TYPE_ID
	{
		GUID TypeId;
	} WNF_TYPE_ID, * PWNF_TYPE_ID;
	typedef const WNF_TYPE_ID* PCWNF_TYPE_ID;

	typedef ULONG WNF_CHANGE_STAMP, * PWNF_CHANGE_STAMP;

#ifndef _X86_

	NTSTATUS
		RtlSubscribeWnfStateChangeNotification(
			_Outptr_ VOID* Subscription,
			_In_ ULONG64 StateName,
			_In_ WNF_CHANGE_STAMP ChangeStamp,
			_In_ decltype(WnfCallback) Callback,
			_In_opt_ size_t CallbackContext,
			_In_opt_ PCWNF_TYPE_ID TypeId,
			_In_opt_ ULONG SerializationGroup,
			_In_opt_ ULONG Unknown);

#endif // !_X86_

	NTSTATUS NTAPI
		RtlUnsubscribeWnfStateChangeNotification(decltype(WnfCallback));

	NTSTATUS NTAPI
		NtQueryWnfStateData(
			_In_ PULONG64 StateName,
			_In_opt_ PWNF_TYPE_ID TypeId,
			_In_opt_ const VOID* ExplicitScope,
			_Out_ PWNF_CHANGE_STAMP ChangeStamp,
			_Out_writes_bytes_to_opt_(*BufferSize, *BufferSize) PVOID Buffer,
			_Inout_ PULONG BufferSize);

	NTSTATUS NTAPI
		NtUpdateWnfStateData(
			_In_ PULONG64 StateName,
			_In_reads_bytes_opt_(Length) const VOID* Buffer,
			_In_opt_ ULONG Length,
			_In_opt_ PCWNF_TYPE_ID TypeId,
			_In_opt_ const PVOID ExplicitScope,
			_In_ WNF_CHANGE_STAMP MatchingChangeStamp,
			_In_ ULONG CheckStamp);
}

#ifdef _X86_

typedef NTSTATUS(__stdcall *x86_RtlSubscribeWnfStateChangeNotification)
(_Outptr_ VOID* Subscription,
	_In_ ULONG64 StateName,
	_In_ WNF_CHANGE_STAMP ChangeStamp,
	_In_ decltype(WnfCallback) Callback,
	_In_opt_ size_t CallbackContext,
	_In_opt_ PCWNF_TYPE_ID TypeId,
	_In_opt_ ULONG SerializationGroup,
	_In_opt_ ULONG Unknown);

#endif // _X86_

enum FocusAssistStatus {
	Off,
	PriorityOnly,
	AlarmsOnly
};

class Wnf
{
public:
	static void SubscribeWnf(ULONG64 state_name, decltype(WnfCallback) callback, intptr_t callback_param);
	static FocusAssistStatus GetFocusAssistStatus();
	static bool IsTabletMode();
	static bool IsLocationInUse();
	static int QueryUnreadNotifications();

	static int ToInt32(const byte buffer[])
	{
		return int((buffer[0]) | (buffer[1]) << 8 | (buffer[2]) << 16 | (buffer[3]) << 24);
	}

	static int ToBool(const byte buffer[])
	{
		return bool(buffer[0]);
	}

private:
	static std::vector<unsigned char> QueryWnf(ULONG64 state_name);
};