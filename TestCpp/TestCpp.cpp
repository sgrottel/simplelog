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

int wmain(int argc, wchar_t const* argv[])
{
	using sgrottel::SimpleLog;

	sgrottel::EchoingSimpleLog log;

	SimpleLog::Write(log, "Started %s", SimpleLog::TimeStampA().c_str());

	SimpleLog::Write(log, L"Default Directory: %s", SimpleLog::GetDefaultDirectory().generic_wstring().c_str());
	SimpleLog::Write(log, "Default Name: %s", SimpleLog::GetDefaultName().generic_string().c_str());
	SimpleLog::Write(log, L"Default Retention: %s", SimpleLog::GetDefaultRetention());

	PrintMessage(log, L"And now for something completely different:");
	SimpleLog::Error(log, "An Error");
	SimpleLog::Warning(log, L"A Warning");
	SimpleLog::Write(log, std::string{"And a normal Message"});

	SimpleLog::Write(&log, "Formatting away: %s %S %s %s %S", "The", L"quick", "Fox", "doesn't", L"care!");

	SimpleLog::Write(log, L"Arg: %s", (argc > 1) ? argv[1] : L"none");

	log.Write("Done.");
}
