using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimpleLogTest
{
	[TestClass]
	public class TestCpp32
	{
		[TestMethod]
		public void OneLogFile()
		{
			TestImpl.OneLogFile(ExeManager.TestCpp32, false);
		}

		[TestMethod]
		public void OneLogFileWaiting()
		{
			TestImpl.OneLogFile(ExeManager.TestCpp32, true);
		}

		[TestMethod]
		public void ManyLogFiles()
		{
			TestImpl.ManyLogFiles(ExeManager.TestCpp32);
		}

		[TestMethod]
		public void MultiProcessLogFiles()
		{
			TestImpl.MultiProcessLogFiles(ExeManager.TestCpp32, ExeManager.TestCpp32);
		}

		[TestMethod]
		public void MultiProcessLogFilesToDelete()
		{
			TestImpl.MultiProcessLogFilesToDelete(ExeManager.TestCpp32);
		}
	}
}
