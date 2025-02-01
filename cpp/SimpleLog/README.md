# SGrottel SimpleLog™ for Cpp
Version: 3.1.0

A very simple log files implementation.
Logs strings with time stamps into log files and implements log file retention.
Integrates via source component (e.g. single header file for cpp), not a lib.


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


## Cpp Usage Example
🚧 TODO


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
