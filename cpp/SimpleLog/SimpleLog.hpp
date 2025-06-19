// SimpleLog.hpp
// Version: 3.2.1
//
// Copyright 2022-2025 SGrottel (www.sgrottel.de)
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

#pragma once

#define SIMPLELOG_VER_MAJOR 3
#define SIMPLELOG_VER_MINOR 2
#define SIMPLELOG_VER_PATCH 1
#define SIMPLELOG_VER_BUILD 0

#if !defined(__cplusplus)
#error SimpleLog.hpp must be compiled as cplusplus source code
#endif
#if defined(_MSVC_LANG) && _MSVC_LANG < 201700L
#error SimpleLog.hpp requires cpp std 2017 or newer
#endif

#include <cstdint>
#include <cstdio>
#include <string>
#include <filesystem>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <mutex>
#include <fstream>
#include <clocale>
#include <stdexcept>
#include <string_view>

#include <iostream>

#if !(defined(_WINDOWS_) || defined(_INC_WINDOWS))
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#endif

#ifndef WINSHELLAPI
#include <shlobj_core.h>
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
		/// Major version number constant
		/// </summary>
		static constexpr int const VERSION_MAJOR = 3;

		/// <summary>
		/// Minor version number constant
		/// </summary>
		static constexpr int const VERSION_MINOR = 2;

		/// <summary>
		/// Patch version number constant
		/// </summary>
		static constexpr int const VERSION_PATCH = 1;

		/// <summary>
		/// Build version number constant
		/// </summary>
		static constexpr int const VERSION_BUILD = 0;

		/// <summary>
		/// Flag message as critical error
		/// </summary>
		static constexpr uint32_t const FlagLevelCritial = 0x00000007;

		/// <summary>
		/// Flag message as error
		/// </summary>
		static constexpr uint32_t const FlagLevelError = 0x00000005;

		/// <summary>
		/// Flag message as warning
		/// </summary>
		static constexpr uint32_t const FlagLevelWarning = 0x0000003;

		/// <summary>
		/// Flag message as normal information message
		/// </summary>
		static constexpr uint32_t const FlagLevelMessage = 0x0000000;

		/// <summary>
		/// Flag message as detail information
		/// </summary>
		static constexpr uint32_t const FlagLevelDetail = 0x00000001;

		/// <summary>
		/// Masks the bits of the flags field which are used to specify the message level
		/// </summary>
		static constexpr uint32_t const FlagLevelMask = 0x00000007;

	protected:

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.</param>
		virtual void WriteImpl(uint32_t flags, char const* message, size_t messageLength) const = 0;

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.</param>
		virtual void WriteImpl(uint32_t flags, wchar_t const* message, size_t messageLength) const = 0;

		/// <summary>
		/// Utility function to forward the write arguments to the implementation with another object (unknown class of this base).
		/// </summary>
		template<typename LOG, typename CHAR>
		void ForwardWriteImpl(LOG& log, uint32_t flags, CHAR const* message, size_t messageLength) const
		{
			log.WriteImpl(flags, message, messageLength);
		}

		/// <summary>
		/// Printf-based formatting of a string; must be zero-terminated.
		/// </summary>
		template<typename ...PARAMS>
		static std::string formatString(char const* format, PARAMS&&... params)
		{
			// Visual Cpp specific
			size_t bufSize = _scprintf(format, params...) + 1;
			std::shared_ptr<char> buf{ new char[bufSize], [](char* p) { delete[] p; } };
			size_t end = sprintf_s(buf.get(), bufSize, format, params...);
			buf.get()[(end > 0 && end < bufSize) ? end : 0] = 0;
			return buf.get();
		}

		/// <summary>
		/// Printf-based formatting of a wstring; must be zero-terminated.
		/// </summary>
		template<typename ...PARAMS>
		static std::wstring formatString(wchar_t const* format, PARAMS&&... params)
		{
			// Visual Cpp specific
			size_t bufSize = _scwprintf(format, params...) + 1;
			std::shared_ptr<wchar_t> buf{ new wchar_t[bufSize], [](wchar_t* p) { delete[] p; } };
			size_t end = swprintf_s(buf.get(), bufSize, format, params...);
			buf.get()[(end > 0 && end < bufSize) ? end : 0] = 0;
			return buf.get();
		}

	public:

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string; must be zero-terminated. Expected to NOT contain a new line at the end.</param>
		inline void Write(uint32_t flags, char const* message) const
		{
			this->WriteImpl(flags, message, std::strlen(message));
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string; must be zero-terminated. Expected to NOT contain a new line at the end.</param>
		inline void Write(uint32_t flags, wchar_t const* message) const
		{
			this->WriteImpl(flags, message, std::wcslen(message));
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		template<typename CHAR, typename TRAITS>
		inline void Write(uint32_t flags, std::basic_string_view<CHAR, TRAITS> const& message) const
		{
			this->WriteImpl(flags, message.data(), message.length());
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		template<typename CHAR, typename TRAITS, typename ALLOCATOR>
		inline void Write(uint32_t flags, std::basic_string<CHAR, TRAITS, ALLOCATOR> const& message) const
		{
			this->WriteImpl(flags, message.data(), message.length());
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string; must be zero-terminated. Expected to NOT contain a new line at the end.
		/// Formatting follows the specification of the printf function family.</param>
		/// <param name="p1">The first formatting argument</param>
		/// <param name="...params">Additional formatting arguments</param>
		template<typename PARAM1, typename ...PARAMS>
		inline void Write(uint32_t flags, char const* message, PARAM1&& p1, PARAMS&&... params) const
		{
			this->Write(flags, formatString(message, std::forward<PARAM1>(p1), std::forward<PARAMS>(params)...));
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string; must be zero-terminated. Expected to NOT contain a new line at the end.
		/// Formatting follows the specification of the printf function family.</param>
		/// <param name="p1">The first formatting argument</param>
		/// <param name="...params">Additional formatting arguments</param>
		template<typename PARAM1, typename ...PARAMS>
		inline void Write(uint32_t flags, wchar_t const* message, PARAM1&& p1, PARAMS&&... params) const
		{
			this->Write(flags, formatString(message, std::forward<PARAM1>(p1), std::forward<PARAMS>(params)...));
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.
		/// Formatting follows the specification of the printf function family.</param>
		/// <param name="p1">The first formatting argument</param>
		/// <param name="...params">Additional formatting arguments</param>
		template<typename CHAR, typename TRAITS, typename PARAM1, typename ...PARAMS>
		inline void Write(uint32_t flags, std::basic_string_view<CHAR, TRAITS> const& message, PARAM1&& p1, PARAMS&&... params) const
		{
			this->Write(flags, formatString(std::basic_string<CHAR>{message}.c_str(), std::forward<PARAM1>(p1), std::forward<PARAMS>(params)...));
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.
		/// Formatting follows the specification of the printf function family.</param>
		/// <param name="p1">The first formatting argument</param>
		/// <param name="...params">Additional formatting arguments</param>
		template<typename CHAR, typename TRAITS, typename ALLOCATOR, typename PARAM1, typename ...PARAMS>
		inline void Write(uint32_t flags, std::basic_string<CHAR, TRAITS, ALLOCATOR> const& message, PARAM1&& p1, PARAMS&&... params) const
		{
			this->Write(flags, formatString(message.c_str(), std::forward<PARAM1>(p1), std::forward<PARAMS>(params)...));
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string; must be zero-terminated. Expected to NOT contain a new line at the end.</param>
		/// <param name="...params">Additional formatting arguments, if any. Formatting follows the specification of the printf function family.</param>
		template<typename ...PARAMS>
		inline void Write(char const* message, PARAMS&&... params) const
		{
			this->Write(static_cast<uint32_t>(0), message, std::forward<PARAMS>(params)...);
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string; must be zero-terminated. Expected to NOT contain a new line at the end.</param>
		/// <param name="...params">Additional formatting arguments, if any. Formatting follows the specification of the printf function family.</param>
		template<typename ...PARAMS>
		inline void Write(wchar_t const* message, PARAMS&&... params) const
		{
			this->Write(static_cast<uint32_t>(0), message, std::forward<PARAMS>(params)...);
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="...params">Additional formatting arguments, if any. Formatting follows the specification of the printf function family.</param>
		template<typename CHAR, typename TRAITS, typename ...PARAMS>
		inline void Write(std::basic_string_view<CHAR, TRAITS> const& message, PARAMS&&... params) const
		{
			this->Write(static_cast<uint32_t>(0), message, std::forward<PARAMS>(params)...);
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="...params">Additional formatting arguments, if any. Formatting follows the specification of the printf function family.</param>
		template<typename CHAR, typename TRAITS, typename ALLOCATOR, typename ...PARAMS>
		inline void Write(std::basic_string<CHAR, TRAITS, ALLOCATOR> const& message, PARAMS&&... params) const
		{
			this->Write(static_cast<uint32_t>(0), message, std::forward<PARAMS>(params)...);
		}

	private:

		template<uint32_t LEVEL, typename ...PARAMS>
		inline void Special(uint32_t flags, char const* message, PARAMS&&... params) const
		{
			this->Write(LEVEL | (flags & ~ISimpleLog::FlagLevelMask), message, std::forward<PARAMS>(params)...);
		}

		template<uint32_t LEVEL, typename ...PARAMS>
		inline void Special(uint32_t flags, wchar_t const* message, PARAMS&&... params) const
		{
			this->Write(LEVEL | (flags & ~ISimpleLog::FlagLevelMask), message, std::forward<PARAMS>(params)...);
		}

		template<uint32_t LEVEL, typename CHAR, typename TRAITS, typename ...PARAMS>
		inline void Special(uint32_t flags, std::basic_string_view<CHAR, TRAITS> const& message, PARAMS&&... params) const
		{
			this->Write(LEVEL | (flags & ~ISimpleLog::FlagLevelMask), message, std::forward<PARAMS>(params)...);
		}

		template<uint32_t LEVEL, typename CHAR, typename TRAITS, typename ALLOCATOR, typename ...PARAMS>
		inline void Special(uint32_t flags, std::basic_string<CHAR, TRAITS, ALLOCATOR> const& message, PARAMS&&... params) const
		{
			this->Write(LEVEL | (flags & ~ISimpleLog::FlagLevelMask), message, std::forward<PARAMS>(params)...);
		}

		template<uint32_t LEVEL, typename ...PARAMS>
		inline void Special(char const* message, PARAMS&&... params) const
		{
			this->Write(LEVEL, message, std::forward<PARAMS>(params)...);
		}

		template<uint32_t LEVEL, typename ...PARAMS>
		inline void Special(wchar_t const* message, PARAMS&&... params) const
		{
			this->Write(LEVEL, message, std::forward<PARAMS>(params)...);
		}

		template<uint32_t LEVEL, typename CHAR, typename TRAITS, typename ...PARAMS>
		inline void Special(std::basic_string_view<CHAR, TRAITS> const& message, PARAMS&&... params) const
		{
			this->Write(LEVEL, message, std::forward<PARAMS>(params)...);
		}

		template<uint32_t LEVEL, typename CHAR, typename TRAITS, typename ALLOCATOR, typename ...PARAMS>
		inline void Special(std::basic_string<CHAR, TRAITS, ALLOCATOR> const& message, PARAMS&&... params) const
		{
			this->Write(LEVEL, message, std::forward<PARAMS>(params)...);
		}

	public:

		/// <summary>
		/// Specialization of the Write function. Use the same arguments as with any of the write overloads.
		/// The log level bits will be overwritten, and the message with be logged as critical error message.
		/// </summary>
		template<typename ...PARAMS>
		inline void Critical(PARAMS&&... params) const
		{
			ISimpleLog::Special<ISimpleLog::FlagLevelCritial>(std::forward<PARAMS>(params)...);
		}

		/// <summary>
		/// Specialization of the Write function. Use the same arguments as with any of the write overloads.
		/// The log level bits will be overwritten, and the message with be logged as error message.
		/// </summary>
		template<typename ...PARAMS>
		inline void Error(PARAMS&&... params) const
		{
			ISimpleLog::Special<ISimpleLog::FlagLevelError>(std::forward<PARAMS>(params)...);
		}

		/// <summary>
		/// Specialization of the Write function. Use the same arguments as with any of the write overloads.
		/// The log level bits will be overwritten, and the message with be logged as warning.
		/// </summary>
		template<typename ...PARAMS>
		inline void Warning(PARAMS&&... params) const
		{
			ISimpleLog::Special<ISimpleLog::FlagLevelWarning>(std::forward<PARAMS>(params)...);
		}

		/// <summary>
		/// Specialization of the Write function. Use the same arguments as with any of the write overloads.
		/// The log level bits will be overwritten, and the message with be logged as message.
		/// </summary>
		template<typename ...PARAMS>
		inline void Message(PARAMS&&... params) const
		{
			ISimpleLog::Special<ISimpleLog::FlagLevelMessage>(std::forward<PARAMS>(params)...);
		}

		/// <summary>
		/// Specialization of the Write function. Use the same arguments as with any of the write overloads.
		/// The log level bits will be overwritten, and the message with be logged as detail message.
		/// </summary>
		template<typename ...PARAMS>
		inline void Detail(PARAMS&&... params) const
		{
			ISimpleLog::Special<ISimpleLog::FlagLevelDetail>(std::forward<PARAMS>(params)...);
		}

		ISimpleLog(const ISimpleLog&) = delete;
		ISimpleLog(ISimpleLog&&) = delete;
		ISimpleLog& operator=(const ISimpleLog&) = delete;
		ISimpleLog& operator=(ISimpleLog&&) = delete;

	protected:
		ISimpleLog() = default;
		virtual ~ISimpleLog() = default;
	};

	/// <summary>
	/// A null implementation of ISimpleLog
	/// </summary>
	class NullLog : public ISimpleLog
	{
	public:
		NullLog() = default;

		NullLog(const NullLog&) = delete;
		NullLog(NullLog&&) = delete;
		NullLog& operator=(const NullLog&) = delete;
		NullLog& operator=(NullLog&&) = delete;

	protected:
		void WriteImpl(uint32_t flags, char const* message, size_t messageLength) const override
		{
			// intentionally empty
			// omitting all messages
		}
		void WriteImpl(uint32_t flags, wchar_t const* message, size_t messageLength) const override
		{
			// intentionally empty
			// omitting all messages
		}
	};

	/// <summary>
	/// SimpleLog implementation
	/// </summary>
	class SimpleLog : public ISimpleLog
	{
	private:

		static std::string timeStampA()
		{
			time_t t = std::time(nullptr);
			struct tm now;
			localtime_s(&now, &t);
			return formatString("%d-%.2d-%.2d %.2d:%.2d:%.2dZ", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
		}

		static std::filesystem::path getProcessPath()
		{
			// Visual Cpp specific
			wchar_t filename[MAX_PATH + 1];
			DWORD filenameLen = GetModuleFileNameW(nullptr, filename, MAX_PATH);
			if (filenameLen > 0)
			{
				return std::filesystem::path{ filename, filename + filenameLen };
			}
			filenameLen = GetProcessImageFileNameW(GetCurrentProcess(), filename, MAX_PATH);
			if (filenameLen > 0)
			{
				return std::filesystem::path{ filename, filename + filenameLen };
			}
			if (__argv != nullptr && __argv[0] != nullptr && __argv[0][0] != 0)
			{
				return std::filesystem::path{ __argv[0] };
			}
			if (__wargv != nullptr && __wargv[0] != nullptr && __wargv[0][0] != 0)
			{
				return std::filesystem::path{ __wargv[0] };
			}
			return {};
		}

		/// <summary>
		/// Utility class to release native resources in a RAII fashion
		/// </summary>
		template<typename func_t>
		class scope_exit
		{
		public:
			scope_exit(func_t&& f) : m_f(std::move(f)) {}
			~scope_exit()
			{
				m_f();
			}
		private:
			func_t m_f;
		};

		HANDLE m_file{ INVALID_HANDLE_VALUE };

		static void toUtf8UnderLock(const char*& outUtf8Str, size_t& outUtf8StrLen, const wchar_t* str, size_t len)
		{
			// even if all chars would be 7bit we still would need to copy due to the padding
			static std::string utf8Str;
			outUtf8StrLen = WideCharToMultiByte(CP_UTF8, 0, str, static_cast<int>(len), nullptr, 0, nullptr, nullptr);
			utf8Str.resize(outUtf8StrLen, '\0');
			WideCharToMultiByte(CP_UTF8, 0, str, static_cast<int>(len), utf8Str.data(), static_cast<int>(outUtf8StrLen), nullptr, nullptr);
			outUtf8Str = utf8Str.c_str();
		}

		static void toUtf8UnderLock(const char*& outUtf8Str, size_t& outUtf8StrLen, const char* str, size_t len)
		{
			bool all7Bit = true;
			for (size_t i = 0; i < len; ++i)
			{
				if (str[i] > 127)
				{
					all7Bit = false;
					break;
				}
			}
			if (all7Bit)
			{
				outUtf8Str = str;
				outUtf8StrLen = len;
			}
			else
			{
				// full conversion needed
				int size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str, static_cast<int>(len), nullptr, 0);
				std::wstring wStr(size, '\0');
				MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str, static_cast<int>(len), wStr.data(), size);
				toUtf8UnderLock(outUtf8Str, outUtf8StrLen, wStr.c_str(), size);
			}
		}

		void writeImplUnderLock(uint32_t flags, char const* msgUtf8, size_t msgUtf8Len) const
		{
			// assumptions:
			//  m_file != INVALID_HANDLE_VALUE
			//  msgUtf8 != nullptr
			//  msgUtf8Len > 0
			//  any returns of timeStampA and all local strings and characters in this function are valid UTF8 (7-bit ASCII)
			std::string ts = timeStampA();
			size_t bufSize = ts.size();

			const char* typeStr = "";
			size_t typeStrLen = 0;
			if ((flags & FlagLevelMask) == FlagLevelCritial)
			{
				typeStr = "CRITICAL";
				typeStrLen = 8;
			}
			else if ((flags & FlagLevelMask) == FlagLevelError)
			{
				typeStr = "ERROR";
				typeStrLen = 5;
			}
			else if ((flags & FlagLevelMask) == FlagLevelWarning)
			{
				typeStr = "WARNING";
				typeStrLen = 7;
			}
			else if ((flags & FlagLevelMask) == FlagLevelDetail)
			{
				typeStr = "DETAIL";
				typeStrLen = 6;
			}

			bufSize += typeStrLen;
			bufSize += msgUtf8Len;
			bufSize += 3; // pipe, space, new line

			// reuse static buffer to avoid reallocations
			static std::vector<char> buf;
			buf.resize(bufSize);

			size_t pos = 0;
			memcpy(buf.data(), ts.c_str(), ts.size());
			pos += ts.size();
			buf.data()[pos++] = '|';
			if (typeStrLen > 0)
			{
				memcpy(buf.data() + pos, typeStr, typeStrLen);
				pos += typeStrLen;
			}
			buf.data()[pos++] = ' ';
			memcpy(buf.data() + pos, msgUtf8, msgUtf8Len);
			pos += msgUtf8Len;
			buf.data()[pos] = '\n';

			WriteFile(m_file, buf.data(), static_cast<DWORD>(bufSize), NULL, NULL);
			FlushFileBuffers(m_file);
		}

		/// <summary>
		/// Mutex used to thread-lock all output
		/// </summary>
		mutable std::mutex m_threadLock;

	public:

#if 1 /* REGION: default configuration values */

		/// <summary>
		/// Returns the default directory where log files are stored.
		///
		/// These locations are tested in this priority order:
		/// 1) "%appdata%\LocalLow\sgrottel_simplelog"
		/// 2) "logs" subfolder of the location of the process' executing assembly
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
			auto testCreateFile = [](std::filesystem::path const& path)
				{
					try
					{
						std::filesystem::path file;
						int i = 0;
						do
						{
							i++;
							file = path / ("temp_" + std::to_string(i) + ".tmp");
						}
						while (std::filesystem::exists(file));

						FILE* f = nullptr;
						// Visual Cpp specific
						if (fopen_s(&f, file.generic_string().c_str(), "wb") != 0)
						{
							f = nullptr;
						}
						if (f == nullptr) throw std::runtime_error("failed to open");
						size_t r = fwrite("Hello World", 11, 1, f);
						fclose(f);

						uintmax_t s = std::filesystem::file_size(file);

						std::filesystem::remove(file);

						return s > 0 && r > 0;
					}
					catch (...) {}
					return false;
				};
			std::filesystem::path createdDir;
			auto cleanDirGuard = [&](std::filesystem::path const& otherPath)
				{
					if (!createdDir.empty())
					{
						if (std::filesystem::is_directory(createdDir))
						{
							std::filesystem::remove_all(createdDir);
						}
						createdDir.clear();
					}
					return otherPath;
				};
			std::filesystem::path parent, path;

			PWSTR wPath;
			if (SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, NULL, &wPath) == S_OK)
			{
				parent = wPath;
				CoTaskMemFree(wPath);
				if (std::filesystem::is_directory(parent))
				{
					path = parent / "sgrottel_simplelog";
					if (!std::filesystem::is_directory(path))
					{
						try
						{
							std::filesystem::create_directory(path);
							createdDir = path;
						}
						catch (...) {}
					}
					if (std::filesystem::is_directory(path))
					{
						if (testCreateFile(path))
						{
							return cleanDirGuard(path);
						}
					}
					cleanDirGuard("");
				}
			}

			parent = getProcessPath();
			if (!parent.empty())
			{
				parent = parent.parent_path();
				if (std::filesystem::is_directory(parent))
				{
					path = parent / "logs";
					if (!std::filesystem::is_directory(path))
					{
						try
						{
							std::filesystem::create_directory(path);
							createdDir = path;
						}
						catch (...) {}
					}
					if (std::filesystem::is_directory(path))
					{
						if (testCreateFile(path))
						{
							return cleanDirGuard(path);
						}
					}
					cleanDirGuard("");
					if (testCreateFile(parent))
					{
						return parent;
					}
				}
			}

			parent = std::filesystem::current_path();
			path = parent / "logs";
			if (!std::filesystem::is_directory(path))
			{
				try
				{
					std::filesystem::create_directory(path);
					createdDir = path;
				}
				catch(...) {}
			}
			if (std::filesystem::is_directory(path))
			{
				if (testCreateFile(path))
				{
					return cleanDirGuard(path);
				}
			}
			cleanDirGuard("");

			return parent;
		}

		/// <summary>
		/// Determines the default name for log files of this process.
		/// The value is based on the process' executing assembly.
		/// </summary>
		/// <returns>The default name for log files of this process</returns>
		static std::filesystem::path GetDefaultName()
		{
			std::filesystem::path procPath = getProcessPath();
			if (!procPath.empty())
			{
				return procPath.filename().replace_extension();
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
		SimpleLog() : SimpleLog(GetDefaultDirectory(), GetDefaultName(), GetDefaultRetention()) { }

		/// <summary>
		/// Creates a SimpleLog instance.
		/// </summary>
		/// <param name="directory">The directory where log files are stored</param>
		/// <param name="name">The name for log files of this process without file name extension</param>
		/// <param name="retention">The default log file retention count; must be 2 or larger</param>
		SimpleLog(std::filesystem::path const& directory, std::filesystem::path const& name, int retention)
		{
			// memory-only writer
			if (directory.empty() && name.empty())
			{
				// m_file stays closed
				return;
			}

			// check arguments
			if (directory.empty()) throw std::invalid_argument("directory");
			if (name.empty()) throw std::invalid_argument("name");
			{
				std::string dirStr = directory.string();
				if (std::all_of(dirStr.begin(), dirStr.end(), isspace)) throw std::invalid_argument("directory");
			}
			{
				std::string nameStr = name.string();
				if (std::all_of(nameStr.begin(), nameStr.end(), isspace)) throw std::invalid_argument("name");
			}
			if (retention < 2) throw std::out_of_range("retention must be 2 or larger");

			// setup
			HANDLE logSetupMutex = NULL;
			scope_exit closeLogSetupMutex{ [&logSetupMutex]()
				{
					if (logSetupMutex != NULL)
					{
						::CloseHandle(logSetupMutex);
						logSetupMutex = NULL;
					}
				} };
			// Visual Cpp specific
			logSetupMutex = CreateMutexW(nullptr, FALSE, L"SGROTTEL_SIMPLELOG_CREATION");
			if (logSetupMutex == NULL)
			{
				throw std::runtime_error("Failed to create initializtion mutex");
			}
			WaitForSingleObject(logSetupMutex, INFINITE);
			scope_exit releaseLogSetupMutex{ [&logSetupMutex]()
				{
					if (logSetupMutex != NULL)
					{
						::ReleaseMutex(logSetupMutex);
					}
				} };

			if (!std::filesystem::is_directory(directory))
			{
				if (!std::filesystem::is_directory(directory.parent_path())) throw std::runtime_error("Log directory does not exist");
				std::filesystem::create_directories(directory);
				if (!std::filesystem::is_directory(directory)) throw std::runtime_error("Failed to create log directory");
			}

			std::filesystem::path fn = directory / (name.wstring() + L"." + std::to_wstring(retention - 1) + L".log");
			if (std::filesystem::is_regular_file(fn))
			{
				std::filesystem::remove(fn);
				if (std::filesystem::is_regular_file(fn))
				{
					std::string msg = "Failed to delete old log file '" + fn.string() + "'";
					throw std::runtime_error(msg.c_str());
				}
			}

			for (int i = retention - 1; i > 0; --i)
			{
				std::filesystem::path tfn = directory / (name.wstring() + L"." + std::to_wstring(i) + L".log");
				std::filesystem::path sfn = directory / (name.wstring() + L"." + std::to_wstring(i - 1) + L".log");
				if (i == 1) sfn = sfn = directory / (name.wstring() + L".log");
				if (!std::filesystem::is_regular_file(sfn)) continue;
				if (std::filesystem::is_regular_file(tfn))
				{
					std::string msg = "Log file retention error. Unexpected log file: '" + tfn.string() + "'";
					throw std::runtime_error(msg.c_str());
				}
				std::filesystem::rename(sfn, tfn);
				if (std::filesystem::is_regular_file(sfn))
				{
					std::string msg = "Log file retention error. Unable to move log file: '" + sfn.string() + "'";
					throw std::runtime_error(msg.c_str());
				}
			}

			fn = directory / (name.wstring() + L".log");

			// Share mode `Delete` allows other processes to rename the file while it is being written.
			// This works because this process keeps an open file handle to write messages, and never reopens based on a file name.
			m_file = ::CreateFileW(fn.wstring().c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, NULL, NULL);
			if (m_file == INVALID_HANDLE_VALUE)
			{
				DWORD le = GetLastError();
				std::string msg = "Failed to create log file: " + std::to_string(le);
				throw std::runtime_error(msg.c_str());
			}
			SetFilePointer(m_file, 0, 0, FILE_END);

		}

		SimpleLog(const SimpleLog&) = delete;
		SimpleLog(SimpleLog&&) = delete;
		SimpleLog& operator=(const SimpleLog&) = delete;
		SimpleLog& operator=(SimpleLog&&) = delete;

		virtual ~SimpleLog()
		{
			std::lock_guard<std::mutex> lock{m_threadLock};
			try
			{
				if (m_file != INVALID_HANDLE_VALUE)
				{
					::CloseHandle(m_file);
					m_file = INVALID_HANDLE_VALUE;
				}
			}
			catch (...) {}
		}

		std::filesystem::path GetFilePath() const
		{
			std::lock_guard<std::mutex> lock{ m_threadLock };
			if (m_file == INVALID_HANDLE_VALUE)
			{
				return {};
			}

			std::vector<wchar_t> strBuf;
			DWORD rv = GetFinalPathNameByHandleW(m_file, strBuf.data(), static_cast<DWORD>(strBuf.size()), FILE_NAME_NORMALIZED);
			if (rv == 0)
			{
				DWORD le = GetLastError();
				std::string msg = "Failed query log file path: " + std::to_string(le);
				throw std::runtime_error(msg.c_str());
			}

			if (static_cast<size_t>(rv) > strBuf.size())
			{
				strBuf.resize(rv);
				rv = GetFinalPathNameByHandleW(m_file, strBuf.data(), static_cast<DWORD>(strBuf.size()), FILE_NAME_NORMALIZED);
				if (rv == 0)
				{
					DWORD le = GetLastError();
					std::string msg = "Failed query log file path 2: " + std::to_string(le);
					throw std::runtime_error(msg.c_str());
				}

				if (static_cast<size_t>(rv) > strBuf.size())
				{
					throw std::runtime_error("Failed query log file path 2: memory allocation failure");
				}
			}

			return std::filesystem::path{ strBuf.data(), strBuf.data() + rv };
		}

	protected:
#if 1 /* REGION: implementation of ISampleLog */

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.</param>
		void WriteImpl(uint32_t flags, char const* message, size_t messageLength) const override
		{
			std::lock_guard<std::mutex> lock{m_threadLock};
			if (m_file == INVALID_HANDLE_VALUE) return;

			const char* utf8Str = nullptr;
			size_t utf8StrLen = 0;

			toUtf8UnderLock(utf8Str, utf8StrLen, message, messageLength);

			writeImplUnderLock(flags, utf8Str, utf8StrLen);
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.</param>
		void WriteImpl(uint32_t flags, wchar_t const* message, size_t messageLength) const override
		{
			std::lock_guard<std::mutex> lock{m_threadLock};
			if (m_file == INVALID_HANDLE_VALUE) return;

			const char* utf8Str = nullptr;
			size_t utf8StrLen = 0;

			toUtf8UnderLock(utf8Str, utf8StrLen, message, messageLength);

			writeImplUnderLock(flags, utf8Str, utf8StrLen);
		}

#endif
	};

	/// <summary>
	/// Extention to SimpleLog, which echoes all messages to the console
	/// </summary>
	class EchoingSimpleLog : public ISimpleLog
	{
	private:
		/// <summary>
		/// Implementation to check if this console output should use colors
		/// </summary>
		/// <returns>True if this console output should use colors</returns>
		static bool EvalCanUseConsoleColors()
		{
			static bool doEval = true;
			static bool evalResult = true;

			if (doEval)
			{
				doEval = false;
				HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
				if (hStdOut == INVALID_HANDLE_VALUE)
				{
					evalResult = false;
					return false;
				}
				DWORD mode = 0;
				if (!GetConsoleMode(hStdOut, &mode))
				{
					evalResult = false;
					return false;
				}
				evalResult = (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) == ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			}

			return evalResult;
		}

		bool m_useStdErr = false;
		bool m_useColors = EvalCanUseConsoleColors();
		bool m_echoCriticals = true;
		bool m_echoErrors = true;
		bool m_echoWarnings = true;
		bool m_echoMessages = true;
		bool m_echoDetails = true;

		ISimpleLog& m_baseLog;

		/// <summary>
		/// Mutex used to thread-lock all output
		/// </summary>
		mutable std::mutex m_threadLock;

	public:

		/// <summary>
		/// Flag message to not be echoed to the console
		/// </summary>
		static constexpr uint32_t const FlagDontEcho = 0x00010000;

		/// <summary>
		/// Creates a EchoingSimpleLog with default values for directory, name, and retention
		/// </summary>
		EchoingSimpleLog(ISimpleLog& baseLog) : m_baseLog{ baseLog } {}

		virtual ~EchoingSimpleLog() = default;

		EchoingSimpleLog(const EchoingSimpleLog&) = delete;
		EchoingSimpleLog(EchoingSimpleLog&&) = delete;
		EchoingSimpleLog& operator=(const EchoingSimpleLog&) = delete;
		EchoingSimpleLog& operator=(EchoingSimpleLog&&) = delete;

		/// <summary>
		/// Gets the flag whether or not to use stderr for critical, error, and warning messages.
		/// </summary>
		inline bool GetUseStdErr() const noexcept { return m_useStdErr; }

		/// <summary>
		/// Sets the flag whether or not to use stderr for critical, error, and warning messages.
		/// </summary>
		inline void SetUseStdErr(bool useStdErr) noexcept { m_useStdErr = useStdErr; }

		/// <summary>
		/// Gets the flag whether or not to output colored text if supported.
		/// </summary>
		inline bool GetUseColors() const noexcept { return m_useColors; }

		/// <summary>
		/// Sets the flag whether or not to output colored text if supported.
		/// </summary>
		inline void SetUseColors(bool useColors) noexcept { m_useColors = useColors && EvalCanUseConsoleColors(); }

		/// <summary>
		/// Gets the flag whether or not to echo critical error messages
		/// </summary>
		inline bool GetEchoCriticals() const noexcept { return m_echoCriticals; }

		/// <summary>
		/// Sets the flag whether or not to echo critical error messages
		/// </summary>
		inline void SetEchoCriticals(bool echoCriticals) noexcept { m_echoCriticals = echoCriticals; }

		/// <summary>
		/// Gets the flag whether or not to echo error messages
		/// </summary>
		inline bool GetEchoErrors() const noexcept { return m_echoErrors; }

		/// <summary>
		/// Sets the flag whether or not to echo error messages
		/// </summary>
		inline void SetEchoErrors(bool echoErrors) noexcept { m_echoErrors = echoErrors; }

		/// <summary>
		/// Gets the flag whether or not to echo warning messages
		/// </summary>
		inline bool GetEchoWarnings() const noexcept { return m_echoWarnings; }

		/// <summary>
		/// Sets the flag whether or not to echo warning messages
		/// </summary>
		inline void SetEchoWarnings(bool echoWarnings) noexcept { m_echoWarnings = echoWarnings; }

		/// <summary>
		/// Gets the flag whether or not to echo messages
		/// </summary>
		inline bool GetEchoMessages() const noexcept { return m_echoMessages; }

		/// <summary>
		/// Sets the flag whether or not to echo messages
		/// </summary>
		inline void SetEchoMessages(bool echoMessages) noexcept { m_echoMessages = echoMessages; }

		/// <summary>
		/// Gets the flag whether or not to echo detail messages
		/// </summary>
		inline bool GetEchoDetails() const noexcept { return m_echoDetails; }

		/// <summary>
		/// Sets the flag whether or not to echo detail messages
		/// </summary>
		inline void SetEchoDetails(bool echoDetails) noexcept { m_echoDetails = echoDetails; }

	protected:

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.</param>
		void WriteImpl(uint32_t flags, char const* message, size_t messageLength) const override
		{
			ForwardWriteImpl(m_baseLog, flags, message, messageLength);
			if ((flags & FlagDontEcho) == FlagDontEcho) return;
			uint32_t level = flags & FlagLevelMask;
			if (level == FlagLevelCritial && !m_echoCriticals) return;
			if (level == FlagLevelError && !m_echoErrors) return;
			if (level == FlagLevelWarning && !m_echoWarnings) return;
			if (level == FlagLevelMessage && !m_echoMessages) return;
			if (level == FlagLevelDetail && !m_echoDetails) return;

			{
				std::lock_guard<std::mutex> lock{m_threadLock};
				bool colorSet = false;
				if (level == FlagLevelCritial && m_useColors)
				{
					printf("\x1b[41m\x1b[97m");
					colorSet = true;
				}
				else if (level == FlagLevelError && m_useColors)
				{
					printf("\x1b[40m\x1b[91m");
					colorSet = true;
				}
				else if (level == FlagLevelWarning && m_useColors)
				{
					printf("\x1b[40m\x1b[93m");
					colorSet = true;
				}
				else if (level == FlagLevelDetail && m_useColors)
				{
					printf("\x1b[40m\x1b[90m");
					colorSet = true;
				}

				fprintf(
					m_useStdErr && (level == FlagLevelCritial || level == FlagLevelError || level == FlagLevelWarning)
					? stderr
					: stdout,
					"%.*s\n", static_cast<int>(messageLength), message);

				if (colorSet)
				{
					printf("\x1b[0m");
				}
			}
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.</param>
		void WriteImpl(uint32_t flags, wchar_t const* message, size_t messageLength) const override
		{
			ForwardWriteImpl(m_baseLog, flags, message, messageLength);
			if ((flags & FlagDontEcho) == FlagDontEcho) return;
			uint32_t level = flags & FlagLevelMask;
			if (level == FlagLevelCritial && !m_echoCriticals) return;
			if (level == FlagLevelError && !m_echoErrors) return;
			if (level == FlagLevelWarning && !m_echoWarnings) return;
			if (level == FlagLevelMessage && !m_echoMessages) return;
			if (level == FlagLevelDetail && !m_echoDetails) return;

			{
				std::lock_guard<std::mutex> lock{m_threadLock};
				bool colorSet = false;
				if (level == FlagLevelCritial && m_useColors)
				{
					wprintf(L"\x1b[41m\x1b[97m");
					colorSet = true;
				}
				else if (level == FlagLevelError && m_useColors)
				{
					wprintf(L"\x1b[40m\x1b[91m");
					colorSet = true;
				}
				else if (level == FlagLevelWarning && m_useColors)
				{
					wprintf(L"\x1b[40m\x1b[93m");
					colorSet = true;
				}
				else if (level == FlagLevelDetail && m_useColors)
				{
					wprintf(L"\x1b[40m\x1b[90m");
					colorSet = true;
				}

				fwprintf(
					m_useStdErr && (level == FlagLevelCritial || level == FlagLevelError || level == FlagLevelWarning)
					? stderr
					: stdout,
					L"%.*s\n", static_cast<int>(messageLength), message);

				if (colorSet)
				{
					wprintf(L"\x1b[0m");
				}
			}
		}

	};

	/// <summary>
	/// Extention to SimpleLog, which echoes all messages to DebugOutput
	/// </summary>
	class DebugOutputEchoingSimpleLog : public ISimpleLog
	{
	private:
		ISimpleLog& m_baseLog;
	public:
		DebugOutputEchoingSimpleLog(ISimpleLog& baseLog) : m_baseLog{ baseLog } {}
		virtual ~DebugOutputEchoingSimpleLog() = default;

		DebugOutputEchoingSimpleLog(const DebugOutputEchoingSimpleLog&) = delete;
		DebugOutputEchoingSimpleLog(DebugOutputEchoingSimpleLog&&) = delete;
		DebugOutputEchoingSimpleLog& operator=(const DebugOutputEchoingSimpleLog&) = delete;
		DebugOutputEchoingSimpleLog& operator=(DebugOutputEchoingSimpleLog&&) = delete;

	protected:

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		void WriteImpl(uint32_t flags, char const* message, size_t messageLength) const override
		{
			ForwardWriteImpl(m_baseLog, flags, message, messageLength);
			std::string outputCopy;
			outputCopy.reserve(messageLength + 4 + 2);
			outputCopy += "[";
			switch (flags & ISimpleLog::FlagLevelMask) {
			case ISimpleLog::FlagLevelCritial: outputCopy += "C"; break;
			case ISimpleLog::FlagLevelError: outputCopy += "E"; break;
			case ISimpleLog::FlagLevelWarning: outputCopy += "W"; break;
			case ISimpleLog::FlagLevelMessage: outputCopy += "l"; break;
			case ISimpleLog::FlagLevelDetail: outputCopy += "d"; break;
			default: outputCopy += "."; break;
			}
			outputCopy += "] ";
			outputCopy += std::string_view{ message, messageLength };
			outputCopy += "\n";
			OutputDebugStringA(outputCopy.c_str());
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		void WriteImpl(uint32_t flags, wchar_t const* message, size_t messageLength) const override
		{
			ForwardWriteImpl(m_baseLog, flags, message, messageLength);
			std::wstring outputCopy;
			outputCopy.reserve(messageLength + 4 + 2);
			outputCopy += L"[";
			switch (flags & ISimpleLog::FlagLevelMask) {
			case ISimpleLog::FlagLevelCritial: outputCopy += L"C"; break;
			case ISimpleLog::FlagLevelError: outputCopy += L"E"; break;
			case ISimpleLog::FlagLevelWarning: outputCopy += L"W"; break;
			case ISimpleLog::FlagLevelMessage: outputCopy += L"l"; break;
			case ISimpleLog::FlagLevelDetail: outputCopy += L"d"; break;
			default: outputCopy += L"."; break;
			}
			outputCopy += L"] ";
			outputCopy += std::wstring_view{ message, messageLength };
			outputCopy += L"\n";
			OutputDebugStringW(outputCopy.c_str());
		}
	};
}
