// TestCpp.cpp  SimpleLog  TestCpp
//
// Copyright 2022-2024 SGrottel (www.sgrottel.de)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Second.h"

#include "SimpleLog/SimpleLog.hpp"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

#include <iostream>
#include <string>
#include <sstream>

int wmain(int argc, wchar_t const* argv[])
{
	using sgrottel::SimpleLog;

	bool waitForSemaphore = (argc > 2 && wcscmp(argv[2], L"-wait") == 0);

	wchar_t filenameBuf[MAX_PATH + 1];
	DWORD filenameLen = GetModuleFileNameW(nullptr, filenameBuf, MAX_PATH);
	std::filesystem::path exename{filenameBuf, filenameBuf + filenameLen};

	std::filesystem::path logDir = exename.parent_path() / "log";

	sgrottel::EchoingSimpleLog log{ logDir.generic_wstring(), "TestSimpleLog", 4 };

	time_t t = std::time(nullptr);
	struct tm now;
	localtime_s(&now, &t);
	std::stringstream startMsg;
	startMsg << "Started " << (now.tm_year + 1900)
		<< "-" << std::setfill('0') << std::setw(2) << (now.tm_mon + 1)
		<< "-" << std::setfill('0') << std::setw(2) << now.tm_mday
		<< " " << std::setfill('0') << std::setw(2) << now.tm_hour
		<< ":" << std::setfill('0') << std::setw(2) << now.tm_min
		<< ":" << std::setfill('0') << std::setw(2) << now.tm_sec << "Z";
	log.Write(sgrottel::EchoingSimpleLog::FlagDontEcho, startMsg.str().c_str());

	SimpleLog::Write(log, L"Default Directory: %s", SimpleLog::GetDefaultDirectory().generic_wstring().c_str());
	SimpleLog::Write(log, "Default Name: %s", SimpleLog::GetDefaultName().generic_string().c_str());
	SimpleLog::Write(log, L"Default Retention: %d", SimpleLog::GetDefaultRetention());

	if (waitForSemaphore)
	{
		HANDLE hReadySemaphore = CreateSemaphoreW(nullptr, 0, 1, L"SGROTTEL_SIMPLELOG_TEST_READY");
		if (hReadySemaphore != nullptr)
		{
			HANDLE hWaitSemaphore = CreateSemaphoreW(nullptr, 0, 1, L"SGROTTEL_SIMPLELOG_TEST_WAIT");
			if (hWaitSemaphore != nullptr)
			{
				std::cout << "Signaling being ready" << std::endl;
				LONG pc = -1;
				BOOL b = ReleaseSemaphore(hReadySemaphore, 1, &pc);

				std::cout << "Waiting...";
				DWORD waited = WaitForSingleObject(hWaitSemaphore, 1000 * 60 * 10); // 10min
				CloseHandle(hWaitSemaphore);
				if (waited != WAIT_OBJECT_0)
				{
					std::cout << std::endl;
					std::cerr << "FAILED TO WAIT: " << waited << std::endl;
					SimpleLog::Error(log, "FAILED TO WAIT: %d", static_cast<int>(waited));
					return 1;
				}
				else
				{
					std::cout << "ok" << std::endl;
				}
			}
			CloseHandle(hReadySemaphore);
		}
	}

	PrintMessage(log, L"And now for something completely different:");
	SimpleLog::Error(log, "An Error");
	SimpleLog::Warning(log, L"A Warning");
	SimpleLog::Write(log, std::string{"And a normal Message"});

	SimpleLog::Write(&log, "Formatting away: %s %S %s %s %S", "The", L"quick", "Fox", "doesn't", L"care!");

	SimpleLog::Write(log, L"Arg: %s", (argc > 1) ? argv[1] : L"none");

	log.Write("Done.XYZ", 5);

	return 0;
}
