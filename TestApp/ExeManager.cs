using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Runtime.Versioning;
using System.Text;
using System.Threading.Tasks;

namespace SimpleLogTest
{

	/// <summary>
	/// Utility class to know the paths of the other executables needed to run the tests
	/// </summary>
	internal static class ExeManager
	{
		private static string? testCSharp = null;
		private static string? testCpp = null;

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

		internal static string TestCpp
		{
			get
			{
				if (testCpp == null)
				{
					assertExes();
					if (testCpp == null || System.IO.File.Exists(testCpp) == false)
					{
						throw new Exception("Failed to find TestCpp");
					}
				}
				return testCpp;
			}
		}

		internal enum ImageType
		{
			x86,
			x64
		};

		private static ImageType imageType = ImageType.x64;
		internal static void SetImageType(ImageType imageType)
		{
			if (ExeManager.imageType != imageType)
			{
				testCpp = null;
			}
			ExeManager.imageType = imageType;
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

			if (testCpp == null)
			{
				string p = Path.Combine(dir, "TestCpp" + imageType.ToString() + ".exe");
				if (!File.Exists(p))
				{
					string sp = FindSourceFile(Path.Combine(dir, @"..\..\..\..\TestCpp\bin"), imageType.ToString(), "TestCpp.exe");
					CopyAllFiles(Path.GetDirectoryName(sp), dir);
					foreach (string file in Directory.GetFiles(dir))
					{
						if (Path.GetFileNameWithoutExtension(file) == "TestCpp")
						{
							File.Move(file, Path.Combine(dir, "TestCpp" + imageType.ToString() + Path.GetExtension(file)));
						}
					}
				}
				if (!File.Exists(p))
				{
					throw new FileNotFoundException();
				}
				testCpp = p;
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
			const string configuration = "Release;
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
