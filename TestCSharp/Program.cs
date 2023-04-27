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

using SGrottel;
using System.Reflection;

internal class Program
{
	private static void Main(string[] args)
	{
		var asm = Assembly.GetExecutingAssembly();

		string logDir = Path.Combine(Path.GetDirectoryName(asm.Location) ?? ".", "log");
		string logName = Path.GetFileNameWithoutExtension(asm.Location);

		EchoingSimpleLog log = new(logDir, logName, 4);

		SimpleLog.Write(log, "Started {0:u}", DateTime.Now);

		SimpleLog.Write(log, "Default Directory: {0}", SimpleLog.GetDefaultDirectory());
		SimpleLog.Write(log, "Default Name: {0}", SimpleLog.GetDefaultName());
		SimpleLog.Write(log, "Default Retention: {0}", SimpleLog.GetDefaultRetention());

		log.Write("And now for something completely different:");
		SimpleLog.Error(log, "An Error");
		SimpleLog.Warning(log, "A Warning");
		SimpleLog.Write(log, "And a normal Message");

		SimpleLog.Write(log, "Formatting away: {0} {1} {2} {3} {4}", new object[]{ "The", "quick", "Fox", "doesn't", "care!"});

		SimpleLog.Write(log, "Arg: {0}", (args.Length > 0) ? args[0] : "none");

		log.Write("Done.");
	}
}
