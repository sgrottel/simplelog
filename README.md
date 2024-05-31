# SGrottel SimpleLog™  <img src="images/SimpleLog_x64.png" alt="SimpleLog Icon" align="left" style="height:1.25em;margin-right:0.25em">
A very simple log files implementation.
Logs strings with time stamps into log files and implements log file retention.
Integrates via source component (e.g. single header file for cpp), not a lib.

![GitHub](https://img.shields.io/github/license/sgrottel/simplelog)


## Integrate in CSharp Project
There are two ways to add SimpleLog™ to your CSharp project:

1. Copy and add the subdirectory [./csharp/SimpleLog](./csharp/SimpleLog) to your project.
   While it is sufficient to only add the file [./csharp/SimpleLog/SimpleLog.cs](./csharp/SimpleLog/SimpleLog.cs) to your project, it is recommended to add the whole subdirectory with all files.
   Make sure that `SimpleLog.cs` is compiled as part of your project.
2. TODO: Nuget Package


## Integrate in Cpp Project
There are two ways to add SimpleLog™ to your Cpp project:

1. Copy and add the subdirectory [./cpp/SimpleLog](./cpp/SimpleLog) to your project.
   While it is sufficient to only add the file [./cpp/SimpleLog/SimpleLog.hpp](./cpp/SimpleLog/SimpleLog.hpp) to your project, it is recommended to add the whole subdirectory with all files.
2. TODO: Nuget Package

After integration, you should be able to include the header file via:
```cpp
#include "SimpleLog/SimpleLog.hpp"
```
You might need to adjust your project configurations for a matching include search path.


## CSharp Usage Example

TODO


## Cpp Usage Example

TODO


## License
This project is freely available as open source under the terms of the [Apache License, Version 2.0](LICENSE)

> Copyright 2022-2023, SGrottel (www.sgrottel.de)
>
> Licensed under the Apache License, Version 2.0 (the "License");
> you may not use this file except in compliance with the License.
> You may obtain a copy of the License at
>
> http://www.apache.org/licenses/LICENSE-2.0
>
> Unless required by applicable law or agreed to in writing, software
> distributed under the License is distributed on an "AS IS" BASIS,
> WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
> See the License for the specific language governing permissions and
> limitations under the License.
