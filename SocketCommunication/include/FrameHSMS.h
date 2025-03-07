#pragma once

enum class HSMS_SESSION
{
	DATA,
	SELECT_REQ,
	SELECT_RSP,
	DESELECT_REQ,
	DESELECT_RSP,
	LINKTEST_REQ,
	LINKTEST_RSP,
	REJECT_REQ,
	SEPARATE_REQ,
};

enum class HSMS_STATE
{
	NONE,
	CONNECTED,
	SELECTED,
};

constexpr int HSMS_TIMEOUT = 100;
constexpr int HSMS_SIZE = 10;