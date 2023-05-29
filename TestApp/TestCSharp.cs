namespace SimpleLogTest
{
	[TestClass]
	public class TestCSharp
	{
		[TestMethod]
		public void OneLogFile()
		{
			TestImpl.OneLogFile(ExeManager.TestCSharp, false);
		}

		[TestMethod]
		public void OneLogFileWaiting()
		{
			TestImpl.OneLogFile(ExeManager.TestCSharp, true);
		}

		[TestMethod]
		public void ManyLogFiles()
		{
			TestImpl.ManyLogFiles(ExeManager.TestCSharp);
		}

		[TestMethod]
		public void MultiProcessLogFiles()
		{
			TestImpl.MultiProcessLogFiles(ExeManager.TestCSharp, ExeManager.TestCSharp);
		}

		[TestMethod]
		public void MultiProcessLogFilesToDelete()
		{
			TestImpl.MultiProcessLogFilesToDelete(ExeManager.TestCSharp);
		}
	}
}