// SimpleLog.hpp
// Version: 3.0.0
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

#ifndef _SIMPLELOG_HPP_INCLUDED_
#define _SIMPLELOG_HPP_INCLUDED_
#pragma once

#define SIMPLELOG_VER_MAJOR 3
#define SIMPLELOG_VER_MINOR 0
#define SIMPLELOG_VER_PATCH 0
#define SIMPLELOG_VER_BUILD 0

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
		static constexpr int const VERSION_MINOR = 0;

		/// <summary>
		/// Patch version number constant
		/// </summary>
		static constexpr int const VERSION_PATCH = 0;

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
		virtual void WriteImpl(uint32_t flags, char const* message, size_t messageLength) = 0;

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.</param>
		virtual void WriteImpl(uint32_t flags, wchar_t const* message, size_t messageLength) = 0;

		template<typename LOG, typename CHAR>
		void ForwardWriteImpl(LOG& log, uint32_t flags, CHAR const* message, size_t messageLength)
		{
			log.WriteImpl(flags, message, messageLength);
		}

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

		inline void Write(uint32_t flags, char const* message)
		{
			this->WriteImpl(flags, message, std::strlen(message));
		}

		inline void Write(uint32_t flags, wchar_t const* message)
		{
			this->WriteImpl(flags, message, std::wcslen(message));
		}

		template<typename CHAR, typename TRAITS>
		inline void Write(uint32_t flags, std::basic_string_view<CHAR, TRAITS> const& message)
		{
			this->WriteImpl(flags, message.data(), message.length());
		}

		template<typename CHAR, typename TRAITS, typename ALLOCATOR>
		inline void Write(uint32_t flags, std::basic_string<CHAR, TRAITS, ALLOCATOR> const& message)
		{
			this->WriteImpl(flags, message.data(), message.length());
		}

		template<typename PARAM1, typename ...PARAMS>
		inline void Write(uint32_t flags, char const* message, PARAM1&& p1, PARAMS&&... params)
		{
			this->Write(flags, formatString(message, std::forward<PARAM1>(p1), std::forward<PARAMS>(params)...));
		}

		template<typename PARAM1, typename ...PARAMS>
		inline void Write(uint32_t flags, wchar_t const* message, PARAM1&& p1, PARAMS&&... params)
		{
			this->Write(flags, formatString(message, std::forward<PARAM1>(p1), std::forward<PARAMS>(params)...));
		}

		template<typename CHAR, typename TRAITS, typename PARAM1, typename ...PARAMS>
		inline void Write(uint32_t flags, std::basic_string_view<CHAR, TRAITS> const& message, PARAM1&& p1, PARAMS&&... params)
		{
			this->Write(flags, formatString(std::basic_string<CHAR>{message}.c_str(), std::forward<PARAM1>(p1), std::forward<PARAMS>(params)...));
		}

		template<typename CHAR, typename TRAITS, typename ALLOCATOR, typename PARAM1, typename ...PARAMS>
		inline void Write(uint32_t flags, std::basic_string<CHAR, TRAITS, ALLOCATOR> const& message, PARAM1&& p1, PARAMS&&... params)
		{
			this->Write(flags, formatString(message.c_str(), std::forward<PARAM1>(p1), std::forward<PARAMS>(params)...));
		}

		///// <summary>
		///// Write a message to the log
		///// </summary>
		///// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		///// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		///// If set less than zero, the message string is treated being zero terminated.</param>
		//inline void Write(char const* message, int messageLength = -1)
		//{
		//	this->Write(0, message, messageLength);
		//}

		///// <summary>
		///// Write a message to the log
		///// </summary>
		///// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		///// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		///// If set less than zero, the message string is treated being zero terminated.</param>
		//inline void Write(wchar_t const* message, int messageLength = -1)
		//{
		//	this->Write(0, message, messageLength);
		//}

		//template<typename STRINGTYPE>
		//inline void Write(STRINGTYPE const& message) { this->Write(0, message.c_str(), -1); }

		//template<typename CHARTYPE, typename ...PARAMS>
		//inline void Write(CHARTYPE const* format, PARAMS&&... params) { this->Write(0, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }

		//template<typename STRINGTYPE, typename ...PARAMS>
		//inline void Write(STRINGTYPE const& format, PARAMS&&... params) { this->Write(0, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

		//template<typename STRINGTYPE>
		//inline void Write(uint32_t flags, STRINGTYPE const& message) { this->Write(flags, message.c_str(), -1); }

		//template<typename CHARTYPE, typename ...PARAMS>
		//inline void Write(uint32_t flags, CHARTYPE const* format, PARAMS&&... params) { this->Write(flags, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }

		//template<typename STRINGTYPE, typename ...PARAMS>
		//inline void Write(uint32_t flags, STRINGTYPE const& format, PARAMS&&... params) { this->Write(flags, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

#if 1 /* REGION: region static short-hand function and variant functions */

		//template<typename CHARTYPE>
		//static void Write(ISimpleLog& log, CHARTYPE const* message) { log.Write(0, message, -1); }
		//template<typename STRINGTYPE>
		//static void Write(ISimpleLog& log, STRINGTYPE const& message) { log.Write(0, message.c_str(), -1); }
		//template<typename CHARTYPE, typename ...PARAMS>
		//static void Write(ISimpleLog& log, CHARTYPE const* format, PARAMS&&... params) { log.Write(0, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		//template<typename STRINGTYPE, typename ...PARAMS>
		//static void Write(ISimpleLog& log, STRINGTYPE const& format, PARAMS&&... params) { log.Write(0, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

		//template<typename CHARTYPE>
		//static void Write(ISimpleLog* log, CHARTYPE const* message) { if (log) log->Write(0, message, -1); }
		//template<typename STRINGTYPE>
		//static void Write(ISimpleLog* log, STRINGTYPE const& message) { if (log) log->Write(0, message.c_str(), -1); }
		//template<typename CHARTYPE, typename ...PARAMS>
		//static void Write(ISimpleLog* log, CHARTYPE const* format, PARAMS&&... params) { if (log) log->Write(0, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		//template<typename STRINGTYPE, typename ...PARAMS>
		//static void Write(ISimpleLog* log, STRINGTYPE const& format, PARAMS&&... params) { if (log) log->Write(0, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

		//template<typename CHARTYPE>
		//static void Warning(ISimpleLog& log, CHARTYPE const* message) { log.Write(FlagWarning, message, -1); }
		//template<typename STRINGTYPE>
		//static void Warning(ISimpleLog& log, STRINGTYPE const& message) { log.Write(FlagWarning, message.c_str(), -1); }
		//template<typename CHARTYPE, typename ...PARAMS>
		//static void Warning(ISimpleLog& log, CHARTYPE const* format, PARAMS&&... params) { log.Write(FlagWarning, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		//template<typename STRINGTYPE, typename ...PARAMS>
		//static void Warning(ISimpleLog& log, STRINGTYPE const& format, PARAMS&&... params) { log.Write(FlagWarning, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

		//template<typename CHARTYPE>
		//static void Warning(ISimpleLog* log, CHARTYPE const* message) { if (log) log->Write(FlagWarning, message, -1); }
		//template<typename STRINGTYPE>
		//static void Warning(ISimpleLog* log, STRINGTYPE const& message) { if (log) log->Write(FlagWarning, message.c_str(), -1); }
		//template<typename CHARTYPE, typename ...PARAMS>
		//static void Warning(ISimpleLog* log, CHARTYPE const* format, PARAMS&&... params) { if (log) log->Write(FlagWarning, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		//template<typename STRINGTYPE, typename ...PARAMS>
		//static void Warning(ISimpleLog* log, STRINGTYPE const& format, PARAMS&&... params) { if (log) log->Write(FlagWarning, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

		//template<typename CHARTYPE>
		//static void Error(ISimpleLog& log, CHARTYPE const* message) { log.Write(FlagError, message, -1); }
		//template<typename STRINGTYPE>
		//static void Error(ISimpleLog& log, STRINGTYPE const& message) { log.Write(FlagError, message.c_str(), -1); }
		//template<typename CHARTYPE, typename ...PARAMS>
		//static void Error(ISimpleLog& log, CHARTYPE const* format, PARAMS&&... params) { log.Write(FlagError, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		//template<typename STRINGTYPE, typename ...PARAMS>
		//static void Error(ISimpleLog& log, STRINGTYPE const& format, PARAMS&&... params) { log.Write(FlagError, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

		//template<typename CHARTYPE>
		//static void Error(ISimpleLog* log, CHARTYPE const* message) { if (log) log->Write(FlagError, message, -1); }
		//template<typename STRINGTYPE>
		//static void Error(ISimpleLog* log, STRINGTYPE const& message) { if (log) log->Write(FlagError, message.c_str(), -1); }
		//template<typename CHARTYPE, typename ...PARAMS>
		//static void Error(ISimpleLog* log, CHARTYPE const* format, PARAMS&&... params) { if (log) log->Write(FlagError, formatString(format, std::forward<PARAMS>(params)...).c_str(), -1); }
		//template<typename STRINGTYPE, typename ...PARAMS>
		//static void Error(ISimpleLog* log, STRINGTYPE const& format, PARAMS&&... params) { if (log) log->Write(FlagError, formatString(format.c_str(), std::forward<PARAMS>(params)...).c_str(), -1); }

#endif

	protected:
		virtual ~ISimpleLog() = default;
	};

	/// <summary>
	/// A null implementation of ISimpleLog
	/// </summary>
	class NullLog : public ISimpleLog
	{
	protected:
		void WriteImpl(uint32_t flags, char const* message, size_t messageLength) override
		{
			// intentionally empty
			// omitting all messages
		}
		void WriteImpl(uint32_t flags, wchar_t const* message, size_t messageLength) override
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

		void toUtf8UnderLock(const char*& outUtf8Str, size_t& outUtf8StrLen, const wchar_t* str, size_t len)
		{
			// even if all chars would be 7bit we still would need to copy due to the padding
			static std::string utf8Str;
			outUtf8StrLen = WideCharToMultiByte(CP_UTF8, 0, str, static_cast<int>(len), nullptr, 0, nullptr, nullptr);
			utf8Str.resize(outUtf8StrLen, '\0');
			WideCharToMultiByte(CP_UTF8, 0, str, static_cast<int>(len), utf8Str.data(), static_cast<int>(outUtf8StrLen), nullptr, nullptr);
			outUtf8Str = utf8Str.c_str();
		}

		void toUtf8UnderLock(const char*& outUtf8Str, size_t& outUtf8StrLen, const char* str, size_t len)
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

	protected:

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
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		void WriteImpl(uint32_t flags, char const* message, size_t messageLength) override
		{
			std::lock_guard<std::mutex> lock{m_threadLock};
			if (m_file == INVALID_HANDLE_VALUE) return;

			const char* utf8Str = nullptr;
			size_t utf8StrLen = 0;

			//if (messageLength < 0)
			//{
			//	messageLength = static_cast<int>(strlen(message));
			//}

			toUtf8UnderLock(utf8Str, utf8StrLen, message, messageLength);

			writeImplUnderLock(flags, utf8Str, utf8StrLen);
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		void WriteImpl(uint32_t flags, wchar_t const* message, size_t messageLength) override
		{
			std::lock_guard<std::mutex> lock{m_threadLock};
			if (m_file == INVALID_HANDLE_VALUE) return;

			const char* utf8Str = nullptr;
			size_t utf8StrLen = 0;

			//if (messageLength < 0)
			//{
			//	messageLength = static_cast<int>(wcslen(message));
			//}

			toUtf8UnderLock(utf8Str, utf8StrLen, message, messageLength);

			writeImplUnderLock(flags, utf8Str, utf8StrLen);
		}

#endif
	};

	/// <summary>
	/// Extention to SimpleLog which, which echoes all messages to the console
	/// </summary>
	class EchoingSimpleLog : public ISimpleLog
	{
	private:
		/// <summary>
		/// Implementation to check if this console output should use colors
		/// </summary>
		/// <returns>True if this console output should use colors</returns>
		bool EvalCanUseConsoleColors()
		{
			HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
			if (hStdOut == INVALID_HANDLE_VALUE)
			{
				return false;
			}
			DWORD mode = 0;
			if (!GetConsoleMode(hStdOut, &mode))
			{
				return false;
			}
			return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) == ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		}

		/// <summary>
		/// Checks if this console output should use colors
		/// </summary>
		/// <returns>True if this console output should use colors</returns>
		bool UseConsoleColors()
		{
			static bool useColors = EvalCanUseConsoleColors();
			return useColors;
		}

		ISimpleLog& m_baseLog;

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

	protected:

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		void WriteImpl(uint32_t flags, char const* message, size_t messageLength) override
		{
			ForwardWriteImpl(m_baseLog, flags, message, messageLength);

			std::cout << std::string_view{ message, messageLength } << std::endl;

			//SimpleLog::Write(flags, message, messageLength);
			//if ((flags & FlagDontEcho) != FlagDontEcho)
			//{
			//	std::lock_guard<std::mutex> lock{m_threadLock};
			//	bool color = false;
			//	if ((flags & FlagError) == FlagError)
			//	{
			//		if (UseConsoleColors())
			//		{
			//			printf("\x1b[40m\x1b[91m");
			//			color = true;
			//		}
			//	}
			//	else if ((flags & FlagWarning) == FlagWarning)
			//	{
			//		if (UseConsoleColors())
			//		{
			//			printf("\x1b[40m\x1b[93m");
			//			color = true;
			//		}
			//	}
			//	if (messageLength < 0)
			//		printf("%s\n", message);
			//	else
			//		printf("%.*s\n", messageLength, message);
			//	if (color)
			//	{
			//		printf("\x1b[0m");
			//	}
			//}
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		/// <param name="messageLength">The length of the message string in characters, not including a terminating zero.
		/// If set less than zero, the message string is treated being zero terminated.</param>
		void WriteImpl(uint32_t flags, wchar_t const* message, size_t messageLength) override
		{
			ForwardWriteImpl(m_baseLog, flags, message, messageLength);

			std::wcout << std::wstring_view{ message, messageLength } << std::endl;

			//SimpleLog::Write(flags, message, messageLength);
			//if ((flags & FlagDontEcho) != FlagDontEcho)
			//{
			//	std::lock_guard<std::mutex> lock{m_threadLock};
			//	bool color = false;
			//	if ((flags & FlagError) == FlagError)
			//	{
			//		if (UseConsoleColors())
			//		{
			//			wprintf(L"\x1b[40m\x1b[91m");
			//			color = true;
			//		}
			//	}
			//	else if ((flags & FlagWarning) == FlagWarning)
			//	{
			//		if (UseConsoleColors())
			//		{
			//			wprintf(L"\x1b[40m\x1b[93m");
			//			color = true;
			//		}
			//	}
			//	if (messageLength < 0)
			//		wprintf(L"%s\n", message);
			//	else
			//		wprintf(L"%.*s\n", messageLength, message);
			//	if (color)
			//	{
			//		wprintf(L"\x1b[0m");
			//	}
			//}
		}

	};
}

#endif /* _SIMPLELOG_HPP_INCLUDED_ */
