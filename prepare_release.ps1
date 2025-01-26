# prepare_release.ps1  SimpleLog
#
# Copyright 2022-2025 SGrottel (www.sgrottel.de)
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

param(
	[string]$buildNumber = $null
)

cd $PSScriptRoot
$ErrorActionPreference = "Stop"

if ($buildNumber) {
	Write-Host "Build Number $buildNumber"
} else {
	Write-Host "Build Number empty"
}


# Parse README.md to generate packet README.md files
$readmeSections = @()
$readmeSectionsLevel = @{}
$readme = [string[]](Get-Content .\README.md)
$caption = '';
$lines = @();
$inBlock = $false
$inOmit = $false
foreach ($line in $readme) {
	if ($line.Trim().ToUpper() -eq '<!-- PACKET OMIT START -->') {
		$inOmit = $true
	}
	if ($line.Trim().ToUpper() -eq '<!-- PACKET OMIT END -->') {
		$inOmit = $false
		continue
	}
	if ($inOmit) { continue }
	if ($line.StartsWith('```')) {
		$inBlock = -not $inBlock
		$lines += $line
		continue;
	}
	if ($inBlock) {
		$lines += $line
		continue;
	}
	if ($line -match '^\s*(#{1,})\s*(\S.*)\s*$') {
		if ($lines.count -gt 0) {
			$readmeSections += [PSCustomObject]@{ caption=$caption; lines=$lines };
		}
		$caption = $matches[2];
		$level = $matches[1].length;
		if ($caption -match '^(.*)<img.*>(.*)$') {
			$caption = $matches[1] + $matches[2]
		}
		$readmeSectionsLevel.Add($caption, $level);
		$lines = @();
	} else {
		$lines += $line;
	}
}
if ($lines.count -gt 0) {
	$readmeSections += [PSCustomObject]@{ caption=$caption; lines=$lines };
}

if ($readmeSections.count -lt 2) {
	Write-Error "Failed to parse global README.md"
}


# Check if CSharp or Cpp files are modified
$lines = [string[]](git status --porcelain)
$conflictingChanges = @()
foreach ($line in $lines) {
	if ($line -match '(?i)^\s*\S+\s+(:?cpp|csharp)/simplelog/.*$') {
		$conflictingChanges += $line
	}
}
if ($conflictingChanges.count -gt 0) {
	Write-Error ("There are changes in files to be released:`n" + ($conflictingChanges -join "`n") + "`nCommit changes first and run script again")
}

$githash = ([string[]](git rev-parse HEAD))[0]
Write-Host "Githash: $githash"


# Parse CSharp file for version
$csharpFile = "./csharp/SimpleLog/SimpleLog.cs"
$csharp = [string](Get-Content $csharpFile -Raw)
if (-not ($csharp -match '(?mi)^\s*//\s*version:\s+(\d[\.\d]*)\s*$')) {
	Write-Error "CSharp: failed to parse header comment version"
}
$csharpVer = [System.Version]::Parse($matches[1])
if ($buildNumber) {
	$csharpVer = New-Object System.Version @($csharpVer.Major, $csharpVer.Minor, $csharpVer.Build, $buildNumber)
}

$matches = [Text.RegularExpressions.MatchCollection]([Regex]::Matches($csharp, '(?mi)^\s*const\s+int\s+VERSION_MAJOR\s+=\s+(\d+)\s*;'))
if ($matches.count -ne 1) {
	Write-Error "CSharp: failed to identify exactly one VERSION_MAJOR field"
}
$csharpVerMajor = [int]$matches[0].Groups[1].Value
$matches = [Text.RegularExpressions.MatchCollection]([Regex]::Matches($csharp, '(?mi)^\s*const\s+int\s+VERSION_MINOR\s+=\s+(\d+)\s*;'))
if ($matches.count -ne 1) {
	Write-Error "CSharp: failed to identify exactly one VERSION_MINOR field"
}
$csharpVerMinor = [int]$matches[0].Groups[1].Value
$matches = [Text.RegularExpressions.MatchCollection]([Regex]::Matches($csharp, '(?mi)^\s*const\s+int\s+VERSION_PATCH\s+=\s+(\d+)\s*;'))
if ($matches.count -ne 1) {
	Write-Error "CSharp: failed to identify exactly one VERSION_PATCH field"
}
$csharpVerPatch = [int]$matches[0].Groups[1].Value
$matches = [Text.RegularExpressions.MatchCollection]([Regex]::Matches($csharp, '(?mi)^\s*const\s+int\s+VERSION_BUILD\s+=\s+(\d+)\s*;'))
if ($matches.count -ne 1) {
	Write-Error "CSharp: failed to identify exactly one VERSION_BUILD field"
}
$csharpVerBuild = [int]$matches[0].Groups[1].Value
$versionValues = @( $csharpVerMajor, $csharpVerMinor, $csharpVerPatch )
if ($buildNumber) {
	$csharpVerBuild = $buildNumber
	$versionValues += $buildNumber
} elseif ($csharpVerBuild -ne 0) {
	$versionValues += $csharpVerBuild
}
$csharpVer2 = New-Object System.Version $versionValues

if ($csharpVer -ne $csharpVer2)
{
	Write-Error "CSharp: file defines two different version numbers:`n`tHeader comment: $csharpVer`n`tCode constants: $csharpVer2"
}

# Inject Build Version
if ($buildNumber) {
	Write-Host "Injecting build number into CSharp to full version: $csharpVer"
	$csharp = `
		$csharp -replace '(?mi)^(\s*//\s*version:\s+)\d[\.\d]*(\s*)$', "`${1}$csharpVer`$2" `
		-replace '(?mi)^(\s*public\s+const\s+int\s+VERSION_BUILD\s+=\s+)\d+(\s*;)', "`${1}$buildNumber`$2"
	Set-Content -Value $csharp -Path $csharpFile -NoNewLine -Encoding "utf8NoBOM"
}


# Parse Cpp file for version
$cppFile = "./cpp/SimpleLog/SimpleLog.hpp"
$cpp = [string](Get-Content $cppFile -Raw)
if (-not ($cpp -match '(?mi)^\s*//\s*version:\s+(\d[\.\d]*)\s*$')) {
	Write-Error "Cpp: failed to parse header comment version"
}
$cppVer = [System.Version]::Parse($matches[1])
if ($buildNumber) {
	$cppVer = New-Object System.Version @($cppVer.Major, $cppVer.Minor, $cppVer.Build, $buildNumber)
}

if (-not ($cpp -match '(?mi)^\s*#\s*define\s+SIMPLELOG_VER_MAJOR\s+(\d+)\s*$')) {
	Write-Error "Cpp: failed to define version major value"
}
$cppVerMajor = [int]$matches[1]
if (-not ($cpp -match '(?mi)^\s*#\s*define\s+SIMPLELOG_VER_MINOR\s+(\d+)\s*$')) {
	Write-Error "Cpp: failed to define version minor value"
}
$cppVerMinor = [int]$matches[1]
if (-not ($cpp -match '(?mi)^\s*#\s*define\s+SIMPLELOG_VER_PATCH\s+(\d+)\s*$')) {
	Write-Error "Cpp: failed to define version patch value"
}
$cppVerPatch = [int]$matches[1]
if (-not ($cpp -match '(?mi)^\s*#\s*define\s+SIMPLELOG_VER_BUILD\s+(\d+)\s*$')) {
	Write-Error "Cpp: failed to define version build value"
}
$cppVerBuild = [int]$matches[1]
$versionValues = @( $cppVerMajor, $cppVerMinor, $cppVerPatch )
if ($buildNumber) {
	$cppVerBuild = $buildNumber
	$versionValues += $buildNumber
} elseif ($cppVerBuild -ne 0) {
	$versionValues += $cppVerBuild
}
$cppVer2 = New-Object System.Version $versionValues

if ($cppVer -ne $cppVer2)
{
	Write-Error "Cpp: file defines two different version numbers:`n`tHeader comment: $cppVer`n`tCode defines: $cppVer2"
}

# Inject Build Version
if ($buildNumber) {
	Write-Host "Injecting build number into Cpp to full version: $cppVer"
	$cpp = `
		$cpp -replace '(?mi)^(\s*//\s*version:\s+)\d[\.\d]*(\s*)$', "`${1}$cppVer`$2" `
		-replace '(?mi)^(\s*#\s*define\s+SIMPLELOG_VER_BUILD\s+)\d+(\s)*$', "`${1}$buildNumber`$2"
	Set-Content -Value $cpp -Path $cppFile -NoNewLine -Encoding "utf8NoBOM"
}


# Utility functions
function Update-ComponentSource {
	param(
		[string]$path,
		[string]$githash,
		[System.Version]$version,
		[bool]$forceUpdate
	)
	$json = (Get-Content $path -Raw | ConvertFrom-Json -AsHashTable)
	if ($forceUpdate -or $json['components'][0]['source'][0]['version'] -ne "$version")
	{
		Write-Host "Updating '$path'`n`tGit Hash: $githash`n`tVersion: $version"
		$json['components'][0]['source'][0]['version'] = "$version"
		$json['components'][0]['source'][0]['hash'] = $githash
		$json | ConvertTo-Json -Depth 10 | Foreach-Object { $_ -replace '  ', "`t" -replace "`r`n", "`n" } | Set-Content $path -NoNewLine -Encoding "utf8NoBOM"
	}
}

function Build-ReadMe {
	param(
		[string]$path,
		[string]$nameSelect,
		[string]$nameSkip,
		[System.Version]$version
	)
	[Collections.ArrayList]$lines = @();
	$skipToNextOnLevel = 1000;
	foreach ($section in $readmeSections)
	{
		$cap = $section.caption
		$level = $readmeSectionsLevel[$cap]
		if ($cap.contains($nameSkip)) {
			$skipToNextOnLevel = $level
			continue
		}
		if ($level -gt $skipToNextOnLevel) {
			continue
		}
		$skipToNextOnLevel = 1000;

		$lines += ''.PadLeft($level, '#') + " $cap"
		$lines += $section.lines
	}

	$lines[0] = $lines[0].Trim() + " for $nameSelect"
	$lines.Insert(1, "Version: $version")
	$lines.Insert(2, '')
	Write-Host "Writing $path"
	Set-Content -Path $path -Value (($lines -join "`n") + "`n") -NoNewLine -Encoding "utf8NoBOM"
}

function Update-Nuspec {
	param(
		[string]$path,
		[System.Version]$version
	)
	Write-Host "Updating version in $path"
	$nuspec = New-Object System.XML.XMLDocument
	$nuspec.PreserveWhitespace = $true
	$nuspecPath = Resolve-Path $path
	$nuspec.Load($nuspecPath)
	$nuspec.package.metadata.version = $version.ToString()
	$nuspec.Save($nuspecPath)
}


# CSharp Package
Update-ComponentSource csharp/SimpleLog/ComponentSource.json $githash $csharpVer ([bool]$buildNumber)
Build-ReadMe csharp/SimpleLog/README.md CSharp Cpp $csharpVer
Copy-Item ./LICENSE csharp/SimpleLog/LICENSE
Update-Nuspec SGrottel.SimpleLog.CSharp.nuspec $csharpVer


# Cpp Package
Update-ComponentSource cpp/SimpleLog/ComponentSource.json $githash $cppVer ([bool]$buildNumber)
Build-ReadMe cpp/SimpleLog/README.md Cpp CSharp $cppVer
Copy-Item ./LICENSE cpp/SimpleLog/LICENSE
Update-Nuspec SGrottel.SimpleLog.Cpp.nuspec $cppVer


Write-Host "done."
