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

using System.Diagnostics;

namespace SimpleLogTest
{

	/// <summary>
	/// Utility class to know the paths of the other executables needed to run the tests
	/// </summary>
	internal static class ExeManager
	{
		private static string? testCSharp = null;
		private static string? testCpp32 = null;
		private static string? testCpp64 = null;

		internal static string TestCSharp
		{
			get
			{
				if (testCSharp == null)
				{
					assertExes();
					if (testCSharp == null || System.IO.File.Exists(testCSharp) == false)
					{
						throw new Exception("Failed to find TestCSharp");
					}
				}
				return testCSharp;
			}
		}

		internal static string TestCpp32
		{
			get
			{
				if (testCpp32 == null)
				{
					assertExes();
					if (testCpp32 == null || System.IO.File.Exists(testCpp32) == false)
					{
						throw new Exception("Failed to find TestCpp");
					}
				}
				return testCpp32;
			}
		}

		internal static string TestCpp64
		{
			get
			{
				if (testCpp64 == null)
				{
					assertExes();
					if (testCpp64 == null || System.IO.File.Exists(testCpp64) == false)
					{
						throw new Exception("Failed to find TestCpp");
					}
				}
				return testCpp64;
			}
		}

		private static void assertExes()
		{
			string dir = AppContext.BaseDirectory;

			if (testCSharp == null)
			{
				string p = Path.Combine(dir, "TestCSharp.exe");
				if (!File.Exists(p))
				{
					string sp = FindSourceFile(Path.Combine(dir, @"..\..\..\..\TestCSharp\bin"), null, "TestCSharp.exe");
					CopyAllFiles(Path.GetDirectoryName(sp), dir);
				}
				if (!File.Exists(p))
				{
					throw new FileNotFoundException();
				}
				testCSharp = p;
			}

			if (testCpp32 == null)
			{
				string p = Path.Combine(dir, "TestCpp32.exe");
				if (!File.Exists(p))
				{
					string sp = FindSourceFile(Path.Combine(dir, @"..\..\..\..\TestCpp\bin"), "x86", "TestCpp.exe");
					CopyAllFiles(Path.GetDirectoryName(sp), dir);
					foreach (string file in Directory.GetFiles(dir))
					{
						if (Path.GetFileNameWithoutExtension(file) == "TestCpp")
						{
							File.Move(file, Path.Combine(dir, "TestCpp32" + Path.GetExtension(file)));
						}
					}
				}
				if (!File.Exists(p))
				{
					throw new FileNotFoundException();
				}
				testCpp32 = p;
			}

			if (testCpp64 == null)
			{
				string p = Path.Combine(dir, "TestCpp64.exe");
				if (!File.Exists(p))
				{
					string sp = FindSourceFile(Path.Combine(dir, @"..\..\..\..\TestCpp\bin"), "x64", "TestCpp.exe");
					CopyAllFiles(Path.GetDirectoryName(sp), dir);
					foreach (string file in Directory.GetFiles(dir))
					{
						if (Path.GetFileNameWithoutExtension(file) == "TestCpp")
						{
							File.Move(file, Path.Combine(dir, "TestCpp64" + Path.GetExtension(file)));
						}
					}
				}
				if (!File.Exists(p))
				{
					throw new FileNotFoundException();
				}
				testCpp64 = p;
			}

		}

		private static void CopyAllFiles(string? from, string to)
		{
			if (from == null)
			{
				throw new ArgumentNullException();
			}
			foreach (string srcFile in Directory.GetFiles(from))
			{
				File.Copy(srcFile, Path.Combine(to, Path.GetFileName(srcFile)), true);
			}
		}

		private static string FindSourceFile(string basePath, string? platform, string exeName)
		{
#if DEBUG
			const string configuration = "Debug";
#else
			const string configuration = "Release";
#endif
			string p = Path.Combine(basePath, configuration);
			if (!Directory.Exists(p))
			{
				throw new FileNotFoundException();
			}

			if (platform == null)
			{
				string[] subdirs = Directory.GetDirectories(p);
				if (subdirs.Length == 1)
				{
					p = Path.Combine(p, subdirs[0]);
				}
				else
				{
					throw new Exception("Multiple potential platform directories found.");
				}
			}
			else
			{
				p = Path.Combine(p, platform);
			}
			if (!Directory.Exists(p))
			{
				throw new FileNotFoundException();
			}

			p = Path.Combine(p, exeName);
			if (!File.Exists(p))
			{
				throw new FileNotFoundException();
			}

			return p;
		}

		internal static Process Start(string exe, string? arg, bool wait)
		{
			ProcessStartInfo psi = new ProcessStartInfo(exe);
			psi.FileName = exe;
			psi.ArgumentList.Clear();
			if (wait && arg == null) arg = "";
			if (arg != null) psi.ArgumentList.Add(arg);
			if (wait) psi.ArgumentList.Add("-wait");
			psi.WorkingDirectory = Path.GetDirectoryName(exe);

			return Process.Start(psi) ?? throw new Exception();
		}

		internal static bool StartAndWait(string exe, string? arg, bool wait)
		{
			Process p = Start(exe, arg, wait);
			p.WaitForExit();
			return true;
		}

	}
}
