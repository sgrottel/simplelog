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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimpleLogTest
{
	[TestClass]
	public class TestCpp64
	{
		[TestMethod]
		public void OneLogFile()
		{
			TestImpl.OneLogFile(ExeManager.TestCpp64, false);
		}

		[TestMethod]
		public void OneLogFileWaiting()
		{
			TestImpl.OneLogFile(ExeManager.TestCpp64, true);
		}

		[TestMethod]
		public void ManyLogFiles()
		{
			TestImpl.ManyLogFiles(ExeManager.TestCpp64);
		}

		[TestMethod]
		public void MultiProcessLogFiles()
		{
			TestImpl.MultiProcessLogFiles(ExeManager.TestCpp64, ExeManager.TestCpp64);
		}

		[TestMethod]
		public void MultiProcessLogFilesToDelete()
		{
			TestImpl.MultiProcessLogFilesToDelete(ExeManager.TestCpp64);
		}
	}
}
