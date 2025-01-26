// TestCrossImplementation.cs  SimpleLog  TestApp
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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimpleLogTest
{
	[TestClass]
	public class TestCrossImplementation
	{
		[TestMethod]
		public void MultiProcessLogFilesCSharpAndCpp32()
		{
			TestImpl.MultiProcessLogFiles(ExeManager.TestCSharp, ExeManager.TestCpp32);
		}

		[TestMethod]
		public void MultiProcessLogFilesCSharpAndCpp64()
		{
			TestImpl.MultiProcessLogFiles(ExeManager.TestCSharp, ExeManager.TestCpp64);
		}

		[TestMethod]
		public void MultiProcessLogFilesCpp32AndCSharp()
		{
			TestImpl.MultiProcessLogFiles(ExeManager.TestCpp32, ExeManager.TestCSharp);
		}

		[TestMethod]
		public void MultiProcessLogFilesCpp32AndCpp64()
		{
			TestImpl.MultiProcessLogFiles(ExeManager.TestCpp32, ExeManager.TestCpp64);
		}

		[TestMethod]
		public void MultiProcessLogFilesCpp64AndCSharp()
		{
			TestImpl.MultiProcessLogFiles(ExeManager.TestCpp64, ExeManager.TestCSharp);
		}

		[TestMethod]
		public void MultiProcessLogFilesCpp64AndCpp32()
		{
			TestImpl.MultiProcessLogFiles(ExeManager.TestCpp64, ExeManager.TestCpp32);
		}
	}
}
