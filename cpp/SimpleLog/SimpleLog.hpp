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

// Version: 2.?.?

#include <cstdint>
#include <cstdio>
#include <string>
#include <filesystem>
#include <memory>
#include <cstdlib>
#include <ctime>

#if !(defined(_WINDOWS_) || defined(_INC_WINDOWS))
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#endif
#include <psapi.h>

namespace sgrottel
{

	/// <summary>
	/// Abstract interface class for writing a message
	/// </summary>
	class ISimpleLog
	{
	public:

		/// <summary>
		/// Flag message as warning
		/// </summary>
		static constexpr uint32_t const FlagWarning = 1;

		/// <summary>
		/// Flag message as error
		/// </summary>
		static constexpr uint32_t const FlagError = 2;

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		virtual void Write(char const* message, int messageLength = -1) = 0;

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		virtual void Write(wchar_t const* message, int messageLength = -1) = 0;

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		virtual void Write(uint32_t flags, char const* message, int messageLength = -1) = 0;

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		virtual void Write(uint32_t flags, wchar_t const* message, int messageLength = -1) = 0;

	protected:
		virtual ~ISimpleLog() = default;
	};

	/// <summary>
	/// SimpleLog implementation
	/// </summary>
	class SimpleLog : public ISimpleLog
	{
	private:
		template<typename ...PARAMS>
		static std::string formatString(char const* format, PARAMS&&... params)
		{
			// Visual studio specific
			size_t bufSize = _scprintf(format, params...) + 1;
			std::shared_ptr<char> buf{ new char[bufSize], [](char* p) { delete[] p; }};
			size_t end = sprintf_s(buf.get(), bufSize, format, params...);
			buf.get()[(end > 0 && end < bufSize) ? end : 0] = 0;
			return buf.get();
		}
		template<typename ...PARAMS>
		static std::wstring formatString(wchar_t const* format, PARAMS&&... params)
		{
			// Visual studio specific
			size_t bufSize = _scwprintf(format, params...) + 1;
			std::shared_ptr<wchar_t> buf{ new wchar_t[bufSize], [](wchar_t* p) { delete[] p; }};
			size_t end = swprintf_s(buf.get(), bufSize, format, params...);
			buf.get()[(end > 0 && end < bufSize) ? end : 0] = 0;
			return buf.get();
		}
		static std::string timeStampA()
		{
			time_t t = std::time(nullptr);
			struct tm now;
			localtime_s(&now, &t);
			return formatString("%d-%.2d-%.2d %.2d:%.2d:%.2dZ", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
		}
		static std::wstring timeStampW()
		{
			time_t t = std::time(nullptr);
			struct tm now;
			localtime_s(&now, &t);
			return formatString(L"%d-%.2d-%.2d %.2d:%.2d:%.2dZ", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
		}

	public:

#if 1 /* REGION: region static short-hand function and variant functions */

		template<typename CHARTYPE>
		static void Write(ISimpleLog& log, CHARTYPE const* message) { log.Write(0, message, -1); }
		template<typename STRINGTYPE>
		static void Write(ISimpleLog& log, STRINGTYPE const& message) { log.Write(0, message.c_str(), -1); }
		template<typename CHARTYPE, typename ...PARAMS>
		static void Write(ISimpleLog& log, CHARTYPE const* format, PARAMS&&... params) { log.Write(0, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		template<typename STRINGTYPE, typename ...PARAMS>
		static void Write(ISimpleLog& log, STRINGTYPE const& format, PARAMS&&... params) { log.Write(0, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

		template<typename CHARTYPE>
		static void Write(ISimpleLog* log, CHARTYPE const* message) { if (log) log->Write(0, message, -1); }
		template<typename STRINGTYPE>
		static void Write(ISimpleLog* log, STRINGTYPE const& message) { if (log) log->Write(0, message.c_str(), -1); }
		template<typename CHARTYPE, typename ...PARAMS>
		static void Write(ISimpleLog* log, CHARTYPE const* format, PARAMS&&... params) { if (log) log->Write(0, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		template<typename STRINGTYPE, typename ...PARAMS>
		static void Write(ISimpleLog* log, STRINGTYPE const& format, PARAMS&&... params) { if (log) log->Write(0, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

		template<typename CHARTYPE>
		static void Warning(ISimpleLog& log, CHARTYPE const* message) { log.Write(FlagWarning, message, -1); }
		template<typename STRINGTYPE>
		static void Warning(ISimpleLog& log, STRINGTYPE const& message) { log.Write(FlagWarning, message.c_str(), -1); }
		template<typename CHARTYPE, typename ...PARAMS>
		static void Warning(ISimpleLog& log, CHARTYPE const* format, PARAMS&&... params) { log.Write(FlagWarning, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		template<typename STRINGTYPE, typename ...PARAMS>
		static void Warning(ISimpleLog& log, STRINGTYPE const& format, PARAMS&&... params) { log.Write(FlagWarning, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

		template<typename CHARTYPE>
		static void Warning(ISimpleLog* log, CHARTYPE const* message) { if (log) log->Write(FlagWarning, message, -1); }
		template<typename STRINGTYPE>
		static void Warning(ISimpleLog* log, STRINGTYPE const& message) { if (log) log->Write(FlagWarning, message.c_str(), -1); }
		template<typename CHARTYPE, typename ...PARAMS>
		static void Warning(ISimpleLog* log, CHARTYPE const* format, PARAMS&&... params) { if (log) log->Write(FlagWarning, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		template<typename STRINGTYPE, typename ...PARAMS>
		static void Warning(ISimpleLog* log, STRINGTYPE const& format, PARAMS&&... params) { if (log) log->Write(FlagWarning, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

		template<typename CHARTYPE>
		static void Error(ISimpleLog& log, CHARTYPE const* message) { log.Write(FlagError, message, -1); }
		template<typename STRINGTYPE>
		static void Error(ISimpleLog& log, STRINGTYPE const& message) { log.Write(FlagError, message.c_str(), -1); }
		template<typename CHARTYPE, typename ...PARAMS>
		static void Error(ISimpleLog& log, CHARTYPE const* format, PARAMS&&... params) { log.Write(FlagError, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		template<typename STRINGTYPE, typename ...PARAMS>
		static void Error(ISimpleLog& log, STRINGTYPE const& format, PARAMS&&... params) { log.Write(FlagError, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

		template<typename CHARTYPE>
		static void Error(ISimpleLog* log, CHARTYPE const* message) { if (log) log->Write(FlagError, message, -1); }
		template<typename STRINGTYPE>
		static void Error(ISimpleLog* log, STRINGTYPE const& message) { if (log) log->Write(FlagError, message.c_str(), -1); }
		template<typename CHARTYPE, typename ...PARAMS>
		static void Error(ISimpleLog* log, CHARTYPE const* format, PARAMS&&... params) { if (log) log->Write(FlagError, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		template<typename STRINGTYPE, typename ...PARAMS>
		static void Error(ISimpleLog* log, STRINGTYPE const& format, PARAMS&&... params) { if (log) log->Write(FlagError, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

#endif

#if 1 /* REGION: default configuration values */

		/// <summary>
		/// Returns the default directory where log files are stored.
		///
		/// These locations are tested in this priority order:
		/// 1) "%appdata%\LocalLow\sgrottel_simplelog"
		/// 2) "logs" subfolder of the localion of the process' executing assembly
		/// 3) the localion of the process' executing assembly
		/// 4) "logs" subfolder of the current working directory
		/// 5) the current working directory
		/// </summary>
		/// <returns>The default path where log files are stored</returns>
		/// <remarks>The function creates folders and files to test access rights. It removes all files and folders again.
		/// If the file system access rights allow for creation but not for deletion, empty test files or folders might stay behind.
		/// </remarks>
		static std::filesystem::path GetDefaultDirectory()
		{
			// SGR TODO: Implement

			return "";
		}

		/// <summary>
		/// Determines the default name for log files of this process.
		/// The value is based on the process' executing assembly.
		/// </summary>
		/// <returns>The default name for log files of this process</returns>
		static std::filesystem::path GetDefaultName()
		{
			wchar_t filename[MAX_PATH + 1];
			DWORD filenameLen = GetModuleFileNameW(nullptr, filename, MAX_PATH);
			if (filenameLen > 0)
			{
				return std::filesystem::path{filename, filename + filenameLen}.filename().replace_extension();
			}
			filenameLen = GetProcessImageFileNameW(GetCurrentProcess(), filename, MAX_PATH);
			if (filenameLen > 0)
			{
				return std::filesystem::path{filename, filename + filenameLen}.filename().replace_extension();
			}

			if (__argv != nullptr && __argv[0] != nullptr && __argv[0][0] != 0)
			{
				return std::filesystem::path{__argv[0]}.filename().replace_extension();
			}
			if (__wargv != nullptr && __wargv[0] != nullptr && __wargv[0][0] != 0)
			{
				return std::filesystem::path{__wargv[0]}.filename().replace_extension();
			}

			return std::to_string(GetCurrentProcessId());
		}

		/// <summary>
		/// Gets the default retention, i.e. how many previous log files are kept in the target directory in addition to the current log file
		/// </summary>
		/// <returns>The default log file retention count.</returns>
		static inline constexpr int const GetDefaultRetention()
		{
			return 10;
		}

#endif

		/// <summary>
		/// Creates a SimpleLog with default values for directory, name, and retention
		/// </summary>
		SimpleLog()
		{

			// SGR TODO: Implement

		}

		/// <summary>
		/// Creates a SimpleLog instance.
		/// </summary>
		/// <param name="directory">The directory where log files are stored</param>
		/// <param name="name">The name for log files of this process</param>
		/// <param name="retention">The default log file retention count; must be 2 or larger</param>
		SimpleLog(std::filesystem::path const& directory, std::filesystem::path const& name, int retention)
		{

			// SGR TODO: Implement

		}

		virtual ~SimpleLog()
		{

			// SGR TODO: Implement

		}

#if 1 /* REGION: implementation of ISampleLog */

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		virtual void Write(char const* message, int messageLength = -1) override
		{
			Write(0u, message, messageLength);
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		virtual void Write(wchar_t const* message, int messageLength = -1) override
		{
			Write(0u, message, messageLength);
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		virtual void Write(uint32_t flags, char const* message, int messageLength = -1) override
		{

			// SGR TODO: Implement

		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		virtual void Write(uint32_t flags, wchar_t const* message, int messageLength = -1) override
		{

			// SGR TODO: Implement

		}

#endif
	};

	/// <summary>
	/// Extention to SimpleLog which, which echoes all messages to the console
	/// </summary>
	class EchoingSimpleLog : public SimpleLog
	{
	public:

		/// <summary>
		/// Creates a EchoingSimpleLog with default values for directory, name, and retention
		/// </summary>
		EchoingSimpleLog() : SimpleLog() {}

		/// <summary>
		/// Creates a EchoingSimpleLog instance.
		/// </summary>
		/// <param name="directory">The directory where log files are stored</param>
		/// <param name="name">The name for log files of this process</param>
		/// <param name="retention">The default log file retention count; must be 2 or larger</param>
		template<typename DIRECTORYT, typename NAMET>
		EchoingSimpleLog(DIRECTORYT const& directory, NAMET const& name, int retention) : SimpleLog(directory, name, retention) { }

		virtual ~EchoingSimpleLog() = default;

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		virtual void Write(uint32_t flags, char const* message, int messageLength = -1) override
		{
			if (messageLength < 0)
				printf("%s\n", message);
			else
				printf("%.*s\n", messageLength, message);
			SimpleLog::Write(flags, message, messageLength);
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		virtual void Write(uint32_t flags, wchar_t const* message, int messageLength = -1) override
		{
			if (messageLength < 0)
				wprintf(L"%s\n", message);
			else
				wprintf(L"%.*s\n", messageLength, message);
			SimpleLog::Write(flags, message, messageLength);
		}

		// additional overloads of `Write` in varallel to the two overrides
		using SimpleLog::Write;
	};
}
