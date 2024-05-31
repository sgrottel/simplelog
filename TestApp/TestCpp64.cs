// TestCpp64.cs  SimpleLog  TestApp
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
