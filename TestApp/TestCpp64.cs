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
