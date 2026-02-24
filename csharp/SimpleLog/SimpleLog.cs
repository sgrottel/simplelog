// SimpleLog.cs
// Version: 3.3.0
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

#nullable enable

using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace SGrottel
{

	/// <summary>
	/// SimpleLog interface for writing a message
	/// </summary>
	public interface ISimpleLog
	{
		/// <summary>
		/// Major version number constant
		/// </summary>
		const int VERSION_MAJOR = 3;

		/// <summary>
		/// Minor version number constant
		/// </summary>
		const int VERSION_MINOR = 3;

		/// <summary>
		/// Patch version number constant
		/// </summary>
		const int VERSION_PATCH = 0;

		/// <summary>
		/// Build version number constant
		/// </summary>
		const int VERSION_BUILD = 0;

		/// <summary>
		/// Flag message as critical error
		/// </summary>
		const uint FlagLevelCritical = 0x00000007;

		/// <summary>
		/// Flag message as error
		/// </summary>
		const uint FlagLevelError = 0x00000005;

		/// <summary>
		/// Flag message as warning
		/// </summary>
		const uint FlagLevelWarning = 0x0000003;

		/// <summary>
		/// Flag message as normal information message
		/// </summary>
		const uint FlagLevelMessage = 0x0000000;

		/// <summary>
		/// Flag message as detail information
		/// </summary>
		const uint FlagLevelDetail = 0x00000001;

		/// <summary>
		/// Masks the bits of the flags field which are used to specify the message level
		/// </summary>
		const uint FlagLevelMask = 0x00000007;

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		void Write(string message);

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		void Write(uint flags, string message);
	}

	/// <summary>
	/// Convenience extension methods to log messages with a specific level
	/// </summary>
	public static class ISimpleLogExt
	{
		/// <summary>
		/// Writes a critical error message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public static void Critical(this ISimpleLog? Log, string message)
		{
			Log?.Write(ISimpleLog.FlagLevelCritical, message);
		}

		/// <summary>
		/// Writes a critical error message to the log
		/// </summary>
		/// <param name="flags">The message flags. Level flag bits will be overwritten to FlagLevelCritical</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public static void Critical(this ISimpleLog? Log, uint flags, string message)
		{
			Log?.Write(ISimpleLog.FlagLevelCritical | (flags & ~ISimpleLog.FlagLevelMask), message);
		}

		/// <summary>
		/// Writes a error message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public static void Error(this ISimpleLog? Log, string message)
		{
			Log?.Write(ISimpleLog.FlagLevelError, message);
		}

		/// <summary>
		/// Writes a error message to the log
		/// </summary>
		/// <param name="flags">The message flags. Level flag bits will be overwritten to FlagLevelError</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public static void Error(this ISimpleLog? Log, uint flags, string message)
		{
			Log?.Write(ISimpleLog.FlagLevelError | (flags & ~ISimpleLog.FlagLevelMask), message);
		}

		/// <summary>
		/// Writes a warning message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public static void Warning(this ISimpleLog? Log, string message)
		{
			Log?.Write(ISimpleLog.FlagLevelWarning, message);
		}

		/// <summary>
		/// Writes a warning message to the log
		/// </summary>
		/// <param name="flags">The message flags. Level flag bits will be overwritten to FlagLevelWarning</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public static void Warning(this ISimpleLog? Log, uint flags, string message)
		{
			Log?.Write(ISimpleLog.FlagLevelWarning | (flags & ~ISimpleLog.FlagLevelMask), message);
		}

		/// <summary>
		/// Writes a message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public static void Message(this ISimpleLog? Log, string message)
		{
			Log?.Write(ISimpleLog.FlagLevelMessage, message);
		}

		/// <summary>
		/// Writes a message to the log
		/// </summary>
		/// <param name="flags">The message flags. Level flag bits will be overwritten to FlagLevelMessage</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public static void Message(this ISimpleLog? Log, uint flags, string message)
		{
			Log?.Write(ISimpleLog.FlagLevelMessage | (flags & ~ISimpleLog.FlagLevelMask), message);
		}

		/// <summary>
		/// Writes a detail message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public static void Detail(this ISimpleLog? Log, string message)
		{
			Log?.Write(ISimpleLog.FlagLevelDetail, message);
		}

		/// <summary>
		/// Writes a detail message to the log
		/// </summary>
		/// <param name="flags">The message flags. Level flag bits will be overwritten to FlagLevelDetail</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public static void Detail(this ISimpleLog? Log, uint flags, string message)
		{
			Log?.Write(ISimpleLog.FlagLevelDetail | (flags & ~ISimpleLog.FlagLevelMask), message);
		}
	}

	/// <summary>
	/// A null implementation of ISimpleLog
	/// </summary>
	public class NullLog : ISimpleLog
	{
		public void Write(string message)
		{
			// intentionally empty
			// omitting all messages
		}
		public void Write(uint flags, string message)
		{
			// intentionally empty
			// omitting all messages
		}
	};

	/// <summary>
	/// SimpleLog implementation
	/// </summary>
	public class SimpleLog : ISimpleLog
	{
		#region Ctor

		[DllImport("shell32.dll")]
		private static extern int SHGetKnownFolderPath([MarshalAs(UnmanagedType.LPStruct)] Guid rfid, uint dwFlags, IntPtr hToken, out IntPtr pszPath);

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
		public static string GetDefaultDirectory()
		{
			bool TestCreateFile(string path)
			{
				try
				{
					// Try writing a file
					string filename;
					do
					{
						filename = Path.Combine(path, "temp_" + Path.GetFileName(Path.GetTempFileName()));
					} while (File.Exists(filename));

					File.WriteAllText(filename, "test"); // if `parent` is read only, this is supposed to throw.
					File.Delete(filename);

					return true;
				}
				catch { }
				return false;
			}
			string? parent, path, createdDir = null;

			string CleanDirGuard(string otherPath)
			{
				if (createdDir != null)
				{
					if (Directory.Exists(createdDir))
					{
						Directory.Delete(createdDir);
					}
					createdDir = null;
				}
				return otherPath;
			}

			parent = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
			try
			{
				IntPtr pszPath = IntPtr.Zero;
				// try getting "%appdata%\LocalLow"
				int hr = SHGetKnownFolderPath(new Guid("A520A1A4-1780-4FF6-BD18-167343C5AF16"), 0, IntPtr.Zero, out pszPath);
				if (hr >= 0) parent = Marshal.PtrToStringAuto(pszPath);
				// else parent will stay as "%appdata%\Local"
				if (pszPath != IntPtr.Zero) Marshal.FreeCoTaskMem(pszPath);
			}
			catch { }
			if (Directory.Exists(parent))
			{
				path = Path.Combine(parent, "sgrottel_simplelog");
				if (!Directory.Exists(path))
				{
					try
					{
						Directory.CreateDirectory(createdDir = path);
					}
					catch { }
				}
				if (Directory.Exists(path))
				{
					if (TestCreateFile(path))
						return CleanDirGuard(path);
				}
				CleanDirGuard("");
				if (TestCreateFile(parent))
					return parent;
			}

			parent = AppContext.BaseDirectory;
			if (Directory.Exists(parent))
			{
				path = Path.Combine(parent, "logs");
				if (!Directory.Exists(path))
				{
					try
					{
						Directory.CreateDirectory(createdDir = path);
					}
					catch { }
				}
				if (Directory.Exists(path))
				{
					if (TestCreateFile(path))
						return CleanDirGuard(path);
				}
				CleanDirGuard("");
				if (TestCreateFile(parent))
					return parent;
			}

			parent = Environment.CurrentDirectory;
			path = Path.Combine(parent, "logs");
			if (!Directory.Exists(path))
			{
				try
				{
					Directory.CreateDirectory(createdDir = path);
				}
				catch { }
			}
			if (Directory.Exists(path))
			{
				if (TestCreateFile(path))
					return CleanDirGuard(path);
			}
			CleanDirGuard("");

			return parent;
		}

		/// <summary>
		/// Determines the default name for log files of this process.
		/// The value is based on the process' executing assembly.
		/// </summary>
		/// <returns>The default name for log files of this process</returns>
		public static string GetDefaultName()
		{
			Assembly asm = Assembly.GetEntryAssembly() ?? Assembly.GetExecutingAssembly();
			string? name = asm.GetName().Name;
			if (name == null)
			{
				name = System.Diagnostics.Process.GetCurrentProcess().ProcessName;
			}
			return name;
		}

		/// <summary>
		/// Gets the default retention, i.e. how many previous log files are kept in the target directory in addition to the current log file
		/// </summary>
		/// <returns>The default log file retention count.</returns>
		public static int GetDefaultRetention()
		{
			return 10;
		}

		/// <summary>
		/// Creates a SimpleLog with default values for directory, name, and retention
		/// </summary>
		public SimpleLog() : this(GetDefaultDirectory(), GetDefaultName(), GetDefaultRetention()) { }

		private StreamWriter? writer = null;

		/// <summary>
		/// Creates a SimpleLog instance.
		/// </summary>
		/// <param name="directory">The directory where log files are stored</param>
		/// <param name="name">The name for log files of this process without file name extension</param>
		/// <param name="retention">The default log file retention count; must be 2 or larger</param>
		public SimpleLog(string directory, string name, int retention)
		{
			if (directory == null && name == null)
			{
				writer = null;
				return;
			}

			// check arguments
			if (directory == null) throw new ArgumentNullException("directory");
			if (name == null) throw new ArgumentNullException("name");
			if (string.IsNullOrWhiteSpace(directory)) throw new ArgumentException("directory");
			if (string.IsNullOrWhiteSpace(name)) throw new ArgumentException("name");
			if (retention < 2) throw new ArgumentException("retention");

			using (Mutex logSetupMutex = new Mutex(false, "SGROTTEL_SIMPLELOG_CREATION"))
			{
				logSetupMutex.WaitOne();
				try
				{
					if (!Directory.Exists(directory))
					{
						string? pp = Path.GetDirectoryName(directory);
						if (!Directory.Exists(pp)) throw new Exception("Log directory does not exist");
						Directory.CreateDirectory(directory);
						if (!Directory.Exists(directory)) throw new Exception("Failed to create log directory");
					}

					string fn = Path.Combine(directory, string.Format("{0}.{1}.log", name, retention - 1));
					if (File.Exists(fn))
					{
						File.Delete(fn);
						if (File.Exists(fn)) throw new Exception(string.Format("Failed to delete old log file '{0}'", fn));
					}

					for (int i = retention - 1; i > 0; --i)
					{
						string tfn = Path.Combine(directory, string.Format("{0}.{1}.log", name, i));
						string sfn = Path.Combine(directory, string.Format("{0}.{1}.log", name, i - 1));
						if (i == 1) sfn = Path.Combine(directory, string.Format("{0}.log", name));
						if (!File.Exists(sfn)) continue;
						if (File.Exists(tfn)) throw new Exception(string.Format("Log file retention error. Unexpected log file: '{0}'", tfn));
						File.Move(sfn, tfn);
						if (File.Exists(sfn)) throw new Exception(string.Format("Log file retention error. Unable to move log file: '{0}'", sfn));
					}

					// Share mode `Delete` allows other processes to rename the file while it is being written.
					// This works because this process keeps an open file handle to write messages, and never reopens based on a file name.
					var file = File.Open(
						Path.Combine(directory, string.Format("{0}.log", name)),
						FileMode.OpenOrCreate,
						FileAccess.Write,
						FileShare.Read | FileShare.Delete);
					file.Seek(0, SeekOrigin.End); // append
					writer = new StreamWriter(file, new UTF8Encoding(false));
				}
				finally
				{
					logSetupMutex.ReleaseMutex();
				}
			}
		}
		#endregion

		#region Implement ISimpleLog
		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public virtual void Write(string message) => Write(0, message);

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public virtual void Write(uint flags, string message)
		{
			lock (threadLock)
			{
				if (writer == null) return;
				string type = "";
				switch (flags & ISimpleLog.FlagLevelMask)
				{
					case ISimpleLog.FlagLevelCritical: type = "CRITICAL"; break;
					case ISimpleLog.FlagLevelError: type = "ERROR"; break;
					case ISimpleLog.FlagLevelWarning: type = "WARNING"; break;
					case ISimpleLog.FlagLevelDetail: type = "DETAIL"; break;
				}
				writer.WriteLine("{0:u}|{2} {1}", DateTime.Now, message, type);
				writer.Flush();
			}
		}
		#endregion

		/// <summary>
		/// Object used to thread-lock all output
		/// </summary>
		private object threadLock = new object();

	}

	/// <summary>
	/// Extention to SimpleLog, which echoes all messages to the console
	/// </summary>
	public class EchoingSimpleLog : ISimpleLog
	{

		/// <summary>
		/// Flag message to not be echoed to the console
		/// </summary>
		public const uint FlagDontEcho = 0x00010000;

		private ISimpleLog baseLog;

		/// <summary>
		/// Creates a EchoingSimpleLog with default values for directory, name, and retention
		/// </summary>
		public EchoingSimpleLog(ISimpleLog baseLog)
		{
			this.baseLog = baseLog;
		}

		/// <summary>
		/// If set to `true`, critical, error, and warning messages will be echoed to `Console.Error`.
		/// For normal and detail messages or if this is set to `false` the messages will be echoed to `Console.Out`
		/// </summary>
		public bool UseErrorOut { get; set; } = false;

		/// <summary>
		/// If set to `true`,
		/// critical messages will be printed in whit on red,
		/// error messages will be printed in red on black,
		/// warnings will be printed as yellow on black, and
		/// details will be printe as dark gray on black.
		/// </summary>
		public bool UseColor { get; set; } = true;

		/// <summary>
		/// If set to `false` no critical messages will be echoed
		/// </summary>
		public bool EchoCriticals { get; set; } = true;

		/// <summary>
		/// If set to `false` no error messages will be echoed
		/// </summary>
		public bool EchoErrors { get; set; } = true;

		/// <summary>
		/// If set to `false` no warning messages will be echoed
		/// </summary>
		public bool EchoWarnings { get; set; } = true;

		/// <summary>
		/// If set to `false` no normal-leveled messages will be echoed
		/// </summary>
		public bool EchoMessages { get; set; } = true;

		/// <summary>
		/// If set to `false` no detail messages will be echoed
		/// </summary>
		public bool EchoDetails { get; set; } = true;

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public void Write(string message) => Write(0, message);

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public void Write(uint flags, string message)
		{
			baseLog.Write(flags, message);
			if ((flags & FlagDontEcho) == FlagDontEcho) return;
			uint level = flags & ISimpleLog.FlagLevelMask;
			if (level == ISimpleLog.FlagLevelCritical && !EchoCriticals) return;
			if (level == ISimpleLog.FlagLevelError && !EchoErrors) return;
			if (level == ISimpleLog.FlagLevelWarning && !EchoWarnings) return;
			if (level == ISimpleLog.FlagLevelMessage  && !EchoMessages) return;
			if (level == ISimpleLog.FlagLevelDetail && !EchoDetails) return;

			lock (threadLock)
			{
				bool colorSet = false;
				if (UseColor)
				{
					switch (level)
					{
						case ISimpleLog.FlagLevelCritical:
							Console.ForegroundColor = ConsoleColor.White;
							Console.BackgroundColor = ConsoleColor.Red;
							colorSet = true;
							break;
						case ISimpleLog.FlagLevelError:
							Console.ForegroundColor = ConsoleColor.Red;
							Console.BackgroundColor = ConsoleColor.Black;
							colorSet = true;
							break;
						case ISimpleLog.FlagLevelWarning:
							Console.ForegroundColor = ConsoleColor.Yellow;
							Console.BackgroundColor = ConsoleColor.Black;
							colorSet = true;
							break;
						case ISimpleLog.FlagLevelDetail:
							Console.ForegroundColor = ConsoleColor.DarkGray;
							Console.BackgroundColor = ConsoleColor.Black;
							colorSet = true;
							break;
					}
				}

				(UseErrorOut && ((level == ISimpleLog.FlagLevelCritical) || (level == ISimpleLog.FlagLevelError) || (level == ISimpleLog.FlagLevelWarning))
					? Console.Error : Console.Out).WriteLine(message);

				if (colorSet)
				{
					Console.ResetColor();
				}
			}
		}

		/// <summary>
		/// Object used to thread-lock all output
		/// </summary>
		private object threadLock = new object();
	}

	/// <summary>
	/// Extention to SimpleLog, which echoes all messages to DebugOutput
	/// </summary>
	public class DebugOutputEchoingSimpleLog : ISimpleLog
	{

		private ISimpleLog baseLog;

		/// <summary>
		/// Creates a EchoingSimpleLog with default values for directory, name, and retention
		/// </summary>
		public DebugOutputEchoingSimpleLog(ISimpleLog baseLog)
		{
			this.baseLog = baseLog;
		}

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public void Write(string message) => Write(0, message);

		/// <summary>
		/// Write a message to the log
		/// </summary>
		/// <param name="flags">The message flags</param>
		/// <param name="message">The message string. Expected to NOT contain a new line at the end.</param>
		public void Write(uint flags, string message)
		{
			baseLog.Write(flags, message);
			char levelMarker;
			switch (flags & ISimpleLog.FlagLevelMask)
			{
				case ISimpleLog.FlagLevelCritical: levelMarker = 'C'; break;
				case ISimpleLog.FlagLevelError: levelMarker = 'E'; break;
				case ISimpleLog.FlagLevelWarning: levelMarker = 'W'; break;
				case ISimpleLog.FlagLevelMessage: levelMarker = 'l'; break;
				case ISimpleLog.FlagLevelDetail: levelMarker = 'd'; break;
				default: levelMarker = '.'; break;
			}
			System.Diagnostics.Debug.WriteLine($"[{levelMarker}] {message}");
		}
	}

}
