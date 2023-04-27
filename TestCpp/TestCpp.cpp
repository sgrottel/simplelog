// Copyright 2022-2023 SGrottel (www.sgrottel.de)
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include "Second.h"

#include "SimpleLog/SimpleLog.hpp"

#include <iostream>
#include <string>
#include <sstream>

int wmain(int argc, wchar_t const* argv[])
{
	using sgrottel::SimpleLog;

	wchar_t filenameBuf[MAX_PATH + 1];
	DWORD filenameLen = GetModuleFileNameW(nullptr, filenameBuf, MAX_PATH);
	std::filesystem::path exename{filenameBuf, filenameBuf + filenameLen};

	std::filesystem::path logDir = exename.parent_path() / "log";

	sgrottel::EchoingSimpleLog log{ logDir.generic_wstring(), "TestSimpleLog", 4 };

	time_t t = std::time(nullptr);
	struct tm now;
	localtime_s(&now, &t);
	std::stringstream timestamp;
	timestamp << (now.tm_year + 1900)
		<< "-" << std::setfill('0') << std::setw(2) << (now.tm_mon + 1)
		<< "-" << std::setfill('0') << std::setw(2) << now.tm_mday
		<< " " << std::setfill('0') << std::setw(2) << now.tm_hour
		<< ":" << std::setfill('0') << std::setw(2) << now.tm_min
		<< ":" << std::setfill('0') << std::setw(2) << now.tm_sec << "Z";
	SimpleLog::Write(log, "Started %s", timestamp.str().c_str());

	SimpleLog::Write(log, L"Default Directory: %s", SimpleLog::GetDefaultDirectory().generic_wstring().c_str());
	SimpleLog::Write(log, "Default Name: %s", SimpleLog::GetDefaultName().generic_string().c_str());
	SimpleLog::Write(log, L"Default Retention: %d", SimpleLog::GetDefaultRetention());

	PrintMessage(log, L"And now for something completely different:");
	SimpleLog::Error(log, "An Error");
	SimpleLog::Warning(log, L"A Warning");
	SimpleLog::Write(log, std::string{"And a normal Message"});

	SimpleLog::Write(&log, "Formatting away: %s %S %s %s %S", "The", L"quick", "Fox", "doesn't", L"care!");

	SimpleLog::Write(log, L"Arg: %s", (argc > 1) ? argv[1] : L"none");

	log.Write("Done.XYZ", 5);
}
