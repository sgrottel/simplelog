// TestCSharp.cs  SimpleLog  TestCSharp
//
// Copyright 2022-2024 SGrottel (www.sgrottel.de)
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
using System.Reflection;
using System.Reflection.Metadata;

internal class TestCSharp
{
	private static int Main(string[] args)
	{
		bool waitForSemaphore = (args.Length > 1 && args[1] == "-wait");

		var asm = Assembly.GetExecutingAssembly();

		string logDir = Path.Combine(Path.GetDirectoryName(asm.Location) ?? ".", "log");

		EchoingSimpleLog log = new(logDir, "TestSimpleLog", 4);

		log.Write(EchoingSimpleLog.FlagDontEcho, string.Format("Started {0:u}", DateTime.Now));

		SimpleLog.Write(log, "Default Directory: {0}", SimpleLog.GetDefaultDirectory());
		SimpleLog.Write(log, "Default Name: {0}", SimpleLog.GetDefaultName());
		SimpleLog.Write(log, "Default Retention: {0}", SimpleLog.GetDefaultRetention());

		if (waitForSemaphore)
		{
			using (Semaphore readySemaphore = new Semaphore(0, 1, "SGROTTEL_SIMPLELOG_TEST_READY"))
			{
				readySemaphore.Release(1);
				Console.Write("Signaling being ready");
				using (Semaphore waitSemaphore = new Semaphore(0, 1, "SGROTTEL_SIMPLELOG_TEST_WAIT"))
				{
					Console.Write("Waiting...");
					bool sig = waitSemaphore.WaitOne(TimeSpan.FromMinutes(10));
					if (!sig)
					{
						Console.WriteLine();
						Console.Error.WriteLine("FAILED TO WAIT: timeout");
						SimpleLog.Error(log, "FAILED TO WAIT: timeout");
						return 1;
					}
					else
					{
						Console.WriteLine("ok");
					}
				}
			}
		}

		log.Write("And now for something completely different:");
		SimpleLog.Error(log, "An Error");
		SimpleLog.Warning(log, "A Warning");
		SimpleLog.Write(log, "And a normal Message");

		SimpleLog.Write(log, "Formatting away: {0} {1} {2} {3} {4}", new object[]{ "The", "quick", "Fox", "doesn't", "care!"});

		SimpleLog.Write(log, "Arg: {0}", (args.Length > 0) ? args[0] : "none");

		log.Write("Done.");

		return 0;
	}
}
