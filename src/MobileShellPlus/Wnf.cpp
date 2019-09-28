#include "pch.h"
#include "Wnf.h"


void Wnf::SubscribeWnf(ULONG64 state_name, decltype(WnfCallback) callback, intptr_t callback_param)
{
	uint32_t buf1{};
	size_t buf2{};
	NTSTATUS result = RtlSubscribeWnfStateChangeNotification(&buf2, state_name, buf1, WnfCallback, callback_param, 0, 0, 1);
}

FocusAssistStatus Wnf::GetFocusAssistStatus()
{
	auto vector = QueryWnf(WNF_SHEL_QUIETHOURS_ACTIVE_PROFILE_CHANGED);

	if (vector[0] == 0x0)
		return FocusAssistStatus::Off;
	if (vector[0] == 0x1)
		return FocusAssistStatus::PriorityOnly;
	if (vector[0] == 0x2)
		return FocusAssistStatus::AlarmsOnly;

	return FocusAssistStatus::Off; //illegal
}

bool Wnf::IsTabletMode()
{
	auto vector = QueryWnf(WNF_TMCN_ISTABLETMODE);
	return ToBool(vector.data());
}

bool Wnf::IsLocationInUse()
{
	auto vector = QueryWnf(WNF_LFS_STATE);
	return ToBool(vector.data());
}

int Wnf::QueryUnreadNotifications()
{
	auto vector = QueryWnf(WNF_SHEL_NOTIFICATIONS);
	return ToInt32(vector.data());
}

std::vector<unsigned char> Wnf::QueryWnf(ULONG64 state_name)
{
	std::vector<unsigned char> wnf_state_buffer(8192);
	unsigned long state_buffer_size = 8192;
	WNF_CHANGE_STAMP wnf_change_stamp;

	NtQueryWnfStateData(&state_name, nullptr, nullptr, &wnf_change_stamp,
		wnf_state_buffer.data(), &state_buffer_size);

	return wnf_state_buffer;
}