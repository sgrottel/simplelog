using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace SimpleLogTest
{
	internal static class LogDirManager
	{
		internal static string Dir
		{
			get
			{
				return Path.Combine(AppContext.BaseDirectory, "log");
			}
		}

		internal static void Delete()
		{
			if (Directory.Exists(Dir))
			{
				Directory.Delete(Dir, true);
			}
		}
	}
}
