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

#include <cstdint> // uint32_t
#include <cstdio>  // snprintf, console output
#include <string>
#include <filesystem>

//#include <iomanip>
//#include <sstream>

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
			return "C";
		}
		template<typename ...PARAMS>
		static std::wstring formatString(wchar_t const* format, PARAMS&&... params)
		{
			return L"W";
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

		static std::filesystem::path GetDefaultDirectory()
		{
			// SGR TODO: Implement

			return "";
		}

		static std::filesystem::path GetDefaultName()
		{
			// SGR TODO: Implement

			return "";
		}

		static inline constexpr int const GetDefaultRetention()
		{
			return 10;
		}

		static std::string TimeStampA()
		{
			// SGR TODO: Implement

			//std::ostringstream oss;
			//auto t = std::time(nullptr);
			//auto tm = *std::localtime(&t);
			//oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");

			return "now";
		}

		static std::wstring TimeStampW()
		{
			// SGR TODO: Implement

			//std::ostringstream oss;
			//auto t = std::time(nullptr);
			//auto tm = *std::localtime(&t);
			//oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");

			return L"now";
		}

#endif

		SimpleLog() {}

		// TODO: Implement ctors

		virtual ~SimpleLog() {}

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
		EchoingSimpleLog() : SimpleLog() {}
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

			// SGR TODO: Implement

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

			// SGR TODO: Implement

			SimpleLog::Write(flags, message, messageLength);
		}

		// additional overloads of `Write` in varallel to the two overrides
		using SimpleLog::Write;
	};
}
