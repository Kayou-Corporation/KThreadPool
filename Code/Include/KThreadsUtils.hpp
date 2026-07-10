#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

#include <Platform.hpp>

#if defined(KLINUX) or defined(KCOMPILER_GCC)
#include <pthread.h>

inline void SetThreadName(std::thread& t, const std::string& name)
{
	pthread_setname_np(t.native_handle(), name.c_str());
}

#elif defined(KWINDOWS)
#include <windows.h>
#include <processthreadsapi.h>

inline void SetThreadName(std::thread& t, const std::string& name)
{
	std::thread::native_handle_type nativeHandle = t.native_handle();
	HANDLE hThread = reinterpret_cast<HANDLE>(nativeHandle);

	if (hThread != INVALID_HANDLE_VALUE)
	{
		SetThreadDescription(hThread, std::wstring(name.begin(), name.end()).c_str());
	}
}
#endif

namespace Kayou
{
	enum class Priority : uint8_t
	{
		High = 0u,
		Low
	};
}
