using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace SimpleLogTest
{
	internal static class TestImpl
	{
		internal static void OneLogFile(string exe, bool wait)
		{
			Assert.IsFalse(string.IsNullOrEmpty(exe));
			Assert.IsTrue(File.Exists(exe));

			LogDirManager.Delete();
			Assert.IsFalse(Directory.Exists(LogDirManager.Dir));

			DateTime timestamp = DateTime.Now;
			string arg = "0x" + Random.Shared.Next(1000000).ToString();

			if (wait)
			{
				System.Diagnostics.Process p1 = ExeManager.Start(exe, arg, true);
				while (!p1.HasExited)
				{
					Thread.Sleep(25);
					ExeManager.StartAndWait(ExeManager.ContinueTest, null, false);
				}
				p1.WaitForExit();
			}
			else
			{
				Assert.IsTrue(ExeManager.StartAndWait(exe, arg, false));
			}

			Assert.IsTrue(Directory.Exists(LogDirManager.Dir));

			string[] files = Directory.GetFiles(LogDirManager.Dir);
			Assert.AreEqual(1, files.Length);

			Assert.AreEqual("TestSimpleLog.log", Path.GetFileName(files[0]));

			AssertLogFileContent(files[0], timestamp, arg);
		}

		internal static void ManyLogFiles(string exe)
		{
			Assert.IsFalse(string.IsNullOrEmpty(exe));
			Assert.IsTrue(File.Exists(exe));

			LogDirManager.Delete();
			Assert.IsFalse(Directory.Exists(LogDirManager.Dir));

			for (int i = 0; i < 10; ++i)
			{
				Assert.IsTrue(ExeManager.StartAndWait(exe, "Iteration " + i.ToString(), false));
				Assert.IsTrue(Directory.Exists(LogDirManager.Dir));
			}

			string[] files = Directory.GetFiles(LogDirManager.Dir);
			Assert.AreEqual(4, files.Length);

			string p0 = Path.Combine(LogDirManager.Dir, "TestSimpleLog.log");
			Assert.IsTrue(File.Exists(p0));
			string p1 = Path.Combine(LogDirManager.Dir, "TestSimpleLog.1.log");
			Assert.IsTrue(File.Exists(p1));
			string p2 = Path.Combine(LogDirManager.Dir, "TestSimpleLog.2.log");
			Assert.IsTrue(File.Exists(p2));
			string p3 = Path.Combine(LogDirManager.Dir, "TestSimpleLog.3.log");
			Assert.IsTrue(File.Exists(p3));

			AssertLogFileContent(p0, null, "Iteration 9");
			AssertLogFileContent(p1, null, "Iteration 8");
			AssertLogFileContent(p2, null, "Iteration 7");
			AssertLogFileContent(p3, null, "Iteration 6");
		}

		private static void AssertLogFileContent(string logfile, DateTime? timestamp, string arg)
		{
			Assert.IsTrue(File.Exists(logfile));
			string[] lines = File.ReadAllLines(logfile);
			Assert.AreEqual(11, lines.Length);

			Regex lineReg = new Regex(@"^(\d{4}-\d{2}-\d{2}\s\d{2}:\d{2}:\d{2}Z)\|(.+)$");

			if (timestamp != null)
			{
				Match m = lineReg.Match(lines[0]);
				Assert.IsTrue(m.Success);
				Assert.AreEqual(3, m.Groups.Count);
				Assert.IsTrue(m.Groups[1].Success);
				Assert.IsTrue(m.Groups[2].Success);
				DateTime lineTime;
				Assert.IsTrue(DateTime.TryParse(m.Groups[1].Value, out lineTime));
				lineTime = lineTime.Subtract(TimeZoneInfo.Local.GetUtcOffset(lineTime));
				Assert.IsTrue(m.Groups[2].Value.StartsWith(" Started "));
				DateTime startTime;
				Assert.IsTrue(DateTime.TryParse(m.Groups[2].Value.Substring(9), out startTime));
				startTime = startTime.Subtract(TimeZoneInfo.Local.GetUtcOffset(startTime));
				Assert.IsTrue((lineTime - timestamp.Value).TotalSeconds < 60.0);
				Assert.IsTrue((startTime - timestamp.Value).TotalSeconds < 60.0);
			}

			for (int i = 0; i < lines.Length; ++i)
			{
				Match m = lineReg.Match(lines[i]);
				Assert.IsTrue(m.Success);
				Assert.AreEqual(3, m.Groups.Count);
				Assert.IsTrue(m.Groups[1].Success);
				Assert.IsTrue(m.Groups[2].Success);
				lines[i] = m.Groups[2].Value;
			}

			Assert.IsTrue(lines[0].StartsWith(" Started "));
			Assert.IsTrue(lines[1].StartsWith(" Default Directory: "));
			Assert.IsTrue(lines[2].StartsWith(" Default Name: "));
			Assert.IsTrue(lines[3].StartsWith(" Default Retention: "));
			Assert.AreEqual(" And now for something completely different:", lines[4]);
			Assert.AreEqual("ERROR An Error", lines[5]);
			Assert.AreEqual("WARNING A Warning", lines[6]);
			Assert.AreEqual(" And a normal Message", lines[7]);
			Assert.AreEqual(" Formatting away: The quick Fox doesn't care!", lines[8]);
			Assert.AreEqual(" Arg: " + arg, lines[9]);
			Assert.AreEqual(" Done.", lines[10]);

		}
	}
}
