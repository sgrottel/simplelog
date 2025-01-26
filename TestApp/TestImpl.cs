// TestImpl.cs  SimpleLog  TestApp
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

using SGrottel;
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
				WaitForTestWaiting();
				SignalTestToContinue();
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
			Assert.AreEqual(12, lines.Length);

			Regex lineReg = new(@"^(\d{4}-\d{2}-\d{2}\s\d{2}:\d{2}:\d{2}Z)\|(.+)$");

			if (timestamp != null)
			{
				Match m = lineReg.Match(lines[0]);
				Assert.IsTrue(m.Success);
				Assert.AreEqual(3, m.Groups.Count);
				Assert.IsTrue(m.Groups[1].Success);
				Assert.IsTrue(m.Groups[2].Success);
				Assert.IsTrue(DateTime.TryParse(m.Groups[1].Value, out DateTime lineTime));
				lineTime = lineTime.Subtract(TimeZoneInfo.Local.GetUtcOffset(lineTime));
				Assert.IsTrue(m.Groups[2].Value.StartsWith(" Started "));
				Assert.IsTrue(DateTime.TryParse(m.Groups[2].Value.AsSpan(9), out DateTime startTime));
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
			Assert.IsTrue(lines[1].StartsWith("DETAIL Default Directory: "));
			Assert.IsTrue(lines[2].StartsWith("DETAIL Default Name: "));
			Assert.IsTrue(lines[3].StartsWith("DETAIL Default Retention: "));
			Assert.AreEqual(" And now for something completely different:", lines[4]);
			Assert.AreEqual("CRITICAL A Critical", lines[5]);
			Assert.AreEqual("ERROR An Error", lines[6]);
			Assert.AreEqual("WARNING A Warning", lines[7]);
			Assert.AreEqual(" And a hidden Message", lines[8]);
			Assert.AreEqual("DETAIL Formatting away: The quick Fox doesn't care!", lines[9]);
			Assert.AreEqual(" Arg: " + arg, lines[10]);
			Assert.AreEqual(" Done.", lines[11]);

		}

		internal static void MultiProcessLogFiles(string exe, string exe2)
		{
			Assert.IsFalse(string.IsNullOrEmpty(exe));
			Assert.IsTrue(File.Exists(exe));
			Assert.IsFalse(string.IsNullOrEmpty(exe2));
			Assert.IsTrue(File.Exists(exe2));

			LogDirManager.Delete();
			Assert.IsFalse(Directory.Exists(LogDirManager.Dir));

			Assert.IsTrue(ExeManager.StartAndWait(exe, "Run 0 Prep", false));
			Assert.IsTrue(Directory.Exists(LogDirManager.Dir));

			string[] files = Directory.GetFiles(LogDirManager.Dir);
			Assert.AreEqual(1, files.Length);
			Assert.IsTrue(File.Exists(Path.Combine(LogDirManager.Dir, "TestSimpleLog.log")));

			var first = ExeManager.Start(exe, "First", true);

			WaitForTestWaiting();

			files = Directory.GetFiles(LogDirManager.Dir);
			Assert.AreEqual(2, files.Length);
			Assert.IsTrue(File.Exists(Path.Combine(LogDirManager.Dir, "TestSimpleLog.log")));
			Assert.IsTrue(File.Exists(Path.Combine(LogDirManager.Dir, "TestSimpleLog.1.log")));
			AssertLogFileContent(Path.Combine(LogDirManager.Dir, "TestSimpleLog.1.log"), null, "Run 0 Prep");

			Assert.IsTrue(ExeManager.StartAndWait(exe2, "Second", false));

			files = Directory.GetFiles(LogDirManager.Dir);
			Assert.AreEqual(3, files.Length);
			Assert.IsTrue(File.Exists(Path.Combine(LogDirManager.Dir, "TestSimpleLog.log")));
			Assert.IsTrue(File.Exists(Path.Combine(LogDirManager.Dir, "TestSimpleLog.1.log")));
			Assert.IsTrue(File.Exists(Path.Combine(LogDirManager.Dir, "TestSimpleLog.2.log")));
			AssertLogFileContent(Path.Combine(LogDirManager.Dir, "TestSimpleLog.log"), null, "Second");
			AssertLogFileContent(Path.Combine(LogDirManager.Dir, "TestSimpleLog.2.log"), null, "Run 0 Prep");

			SignalTestToContinue();
			Assert.IsTrue(first.WaitForExit((int)TimeSpan.FromMinutes(5).TotalMilliseconds));

			files = Directory.GetFiles(LogDirManager.Dir);
			Assert.AreEqual(3, files.Length);
			Assert.IsTrue(File.Exists(Path.Combine(LogDirManager.Dir, "TestSimpleLog.log")));
			Assert.IsTrue(File.Exists(Path.Combine(LogDirManager.Dir, "TestSimpleLog.1.log")));
			Assert.IsTrue(File.Exists(Path.Combine(LogDirManager.Dir, "TestSimpleLog.2.log")));
			AssertLogFileContent(Path.Combine(LogDirManager.Dir, "TestSimpleLog.log"), null, "Second");
			AssertLogFileContent(Path.Combine(LogDirManager.Dir, "TestSimpleLog.1.log"), null, "First");
			AssertLogFileContent(Path.Combine(LogDirManager.Dir, "TestSimpleLog.2.log"), null, "Run 0 Prep");
		}

		private static void WaitForTestWaiting()
		{
			using (Semaphore waitSemaphore = new(0, 1, "SGROTTEL_SIMPLELOG_TEST_READY"))
			{
				bool sig = waitSemaphore.WaitOne(TimeSpan.FromMinutes(10));
				Assert.IsTrue(sig);
			}
		}

		private static void SignalTestToContinue()
		{
			using (Semaphore readySemaphore = new(0, 1, "SGROTTEL_SIMPLELOG_TEST_WAIT"))
			{
				int oc = readySemaphore.Release(1);
				Assert.AreEqual(0, oc);
			}
		}

		internal static void MultiProcessLogFilesToDelete(string exe)
		{
			Assert.IsFalse(string.IsNullOrEmpty(exe));
			Assert.IsTrue(File.Exists(exe));
			LogDirManager.Delete();
			Assert.IsFalse(Directory.Exists(LogDirManager.Dir));

			Assert.IsTrue(ExeManager.StartAndWait(exe, "Run 0 Prep", false));
			Assert.IsTrue(Directory.Exists(LogDirManager.Dir));

			string[] files = Directory.GetFiles(LogDirManager.Dir);
			Assert.AreEqual(1, files.Length);
			Assert.IsTrue(File.Exists(Path.Combine(LogDirManager.Dir, "TestSimpleLog.log")));

			var first = ExeManager.Start(exe, "First", true);

			WaitForTestWaiting();

			for (int i = 0; i < 10; ++i)
			{
				Assert.IsTrue(ExeManager.StartAndWait(exe, "Iteration " + i.ToString(), false));
				Assert.IsTrue(Directory.Exists(LogDirManager.Dir));
			}

			files = Directory.GetFiles(LogDirManager.Dir);
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

			SignalTestToContinue();
			Assert.IsTrue(first.WaitForExit((int)TimeSpan.FromMinutes(5).TotalMilliseconds));

			files = Directory.GetFiles(LogDirManager.Dir);
			Assert.AreEqual(4, files.Length);

			Assert.IsTrue(File.Exists(p0));
			Assert.IsTrue(File.Exists(p1));
			Assert.IsTrue(File.Exists(p2));
			Assert.IsTrue(File.Exists(p3));

			AssertLogFileContent(p0, null, "Iteration 9");
			AssertLogFileContent(p1, null, "Iteration 8");
			AssertLogFileContent(p2, null, "Iteration 7");
			AssertLogFileContent(p3, null, "Iteration 6");

		}
	}
}
