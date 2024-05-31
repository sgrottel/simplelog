# SimpleLog  unify_file_header_comments.ps1
#
# Copyright 2022-2024 SGrottel (www.sgrottel.de)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cd $PSScriptRoot
$ErrorActionPreference = "Stop"

# Parse README.md for the up-to-date license excerpt
$readme = [string[]](Get-Content .\README.md)
$readmeSections = @{};
$caption = '';
$lines = @();
foreach ($line in $readme) {
	if ($line -match "^\s*#{1,}\s*(\S.*)\s*$") {
		if ($lines.count -gt 0) {
			$readmeSections.add($caption, $lines);
		}
		$caption = $matches[1];
		$lines = @();
	} else {
		$lines += $line;
	}
}
if ($lines.count -gt 0) {
	$readmeSections.add($caption, $lines);
}

if (-not $readmeSections.License) {
	Write-Error "Failed to parse `License` section in README file"
}
$license = $readmeSections.License | Where-Object { $_.StartsWith(">") } | Foreach-Object { $_.SubString(1).Trim() }
if ($license.count -lt 10) {
	Write-Error "`License` excerpt too short"
}

# Load all code files and adjust where needed
$files = Get-ChildItem -rec *.cs,*.cpp,*.h,*.hpp,*.ps1 -Exclude *\obj\* | Where-Object { -not $_.Directory.FullName.Contains("\obj\") }
foreach ($file in $files) {
	Write-Host $file.Name

	$lines = [string[]](Get-Content $file)

	switch ($file.Extension.ToLower())
	{
		".cs" {
			$prefix = "//";
		}
		{ @(".cpp", ".h", ".hpp") -contains $_ } {
			$prefix = "//";
		}
		".ps1" {
			$prefix = "#";
		}
		default {
			Write-Error "Unexpected file extension $($file.Extension)"
		}
	}

	if ($lines.count -lt 10) {
		Write-Error "File too short"
	}

	$header = @()
	$content = $false
	foreach ($line in $lines) {
		if (-not $content) {
			if ($line.StartsWith($prefix)) {
				$header += $line
			} else {
				$content = @()
				$content += "$prefix"
				$content += "$line"
			}
		} else {
			$content += $line
		}
	}

	Write-Host "CONTENT:"
	if ($content) {
		$content = $content[1..($content.count)]
	} else {
		$content = @()
	}
	$content

	Write-Host "HEADER:"
	$header

	Write-Host "DONE"

	break;
}


# Write-Error "NOT IMPLEMENTED"
