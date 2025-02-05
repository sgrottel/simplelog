# SGrottel SimpleLog™  <img src="images/SimpleLog_x64.png" alt="SimpleLog Icon" align="left" style="height:1.25em;margin-right:0.25em">
A very simple log files implementation.
Logs strings with time stamps into log files and implements log file retention.
Integrates via source component (e.g. single header file for cpp), not a lib.

<!-- PACKET OMIT START -->
[![GitHub](https://img.shields.io/github/license/sgrottel/simplelog)](LICENSE)
[![GitHub Release](https://img.shields.io/github/v/release/sgrottel/simplelog)](https://github.com/sgrottel/simplelog/releases)
[![Test Apps Action](https://github.com/sgrottel/simplelog/actions/workflows/TestApps.yaml/badge.svg)](https://github.com/sgrottel/simplelog/actions/workflows/TestApps.yaml)
[![NuGet Version](https://img.shields.io/nuget/v/SGrottel.SimpleLog.CSharp?logo=nuget&label=CSharp)](https://www.nuget.org/packages/SGrottel.SimpleLog.CSharp/)
[![NuGet Version](https://img.shields.io/nuget/v/SGrottel.SimpleLog.Cpp?logo=nuget&label=Cpp)](https://www.nuget.org/packages/SGrottel.SimpleLog.Cpp/)

<!-- PACKET OMIT END -->

## Integrate in CSharp Project
There are two ways to add SimpleLog™ to your CSharp project:

1. Copy and add the subdirectory [./csharp/SimpleLog](./csharp/SimpleLog) to your project.
   While it is sufficient to only add the file [./csharp/SimpleLog/SimpleLog.cs](./csharp/SimpleLog/SimpleLog.cs) to your project, it is recommended to add the whole subdirectory with all files.
   Make sure that `SimpleLog.cs` is compiled as part of your project.
2. Add the nuget package [SGrottel.SimpleLog.CSharp](https://www.nuget.org/packages/SGrottel.SimpleLog.CSharp/) to your project


## Integrate in Cpp Project
There are two ways to add SimpleLog™ to your Cpp project:

1. Copy and add the subdirectory [./cpp/SimpleLog](./cpp/SimpleLog) to your project.
   While it is sufficient to only add the file [./cpp/SimpleLog/SimpleLog.hpp](./cpp/SimpleLog/SimpleLog.hpp) to your project, it is recommended to add the whole subdirectory with all files.
2. Add the nuget package [SGrottel.SimpleLog.Cpp](https://www.nuget.org/packages/SGrottel.SimpleLog.Cpp/) to your project

After integration, you should be able to include the header file via:
```cpp
#include "SimpleLog/SimpleLog.hpp"
```
You might need to adjust your project configurations for a matching include search path.


## CSharp Usage Example
🚧 TODO


## Cpp Usage Example
🚧 TODO

<!-- PACKET OMIT START -->

### Note on Char-Array-Pointer Strings + Length
Sometimes, especially in the context of de-/serialization strings might be represented by a pointer to char arrays and a length.
In these cases, the strings in the char array might be explicitly **not zero-terminated**.

This is **not supported** by this library.
Consider using a `string_view` wrapper in those cases:
```cpp
const char* str = ...;
size_t len = ...;
log.Write(0, std::string_view{str, len});
```

Note: if you combine such `string_view` input with log function variants that support argument formatting,
and additional copy of the input string has to be created (using `std::string`) to ensure the input is zero-terminated.

### Note on Formatting via String Streams
To use string-based formatting, you can utilize `stringstream` objects:
```cpp
log.Write(0, (std::stringstream{} << "Value: " << v).str());
```

<!-- PACKET OMIT END -->

## License
This project is freely available as open source under the terms of the [Apache License, Version 2.0](LICENSE)

> Copyright 2022-2025 SGrottel (www.sgrottel.de)
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
