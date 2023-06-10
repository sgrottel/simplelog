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
