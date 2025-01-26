// This file is used by Code Analysis to maintain SuppressMessage
// attributes that are applied to this project.
// Project-level suppressions either have no target or are given
// a specific target and scoped to a namespace, type, member, etc.

using System.Diagnostics.CodeAnalysis;

[assembly: SuppressMessage("Performance", "SYSLIB1045:Convert to 'GeneratedRegexAttribute'.", Justification = "Better locality of variable", Scope = "member", Target = "~M:SimpleLogTest.TestImpl.AssertLogFileContent(System.String,System.Nullable{System.DateTime},System.String)")]
[assembly: SuppressMessage("Style", "IDE0063:Use simple 'using' statement", Justification = "Better readability of scope", Scope = "member", Target = "~M:SimpleLogTest.TestImpl.WaitForTestWaiting")]
[assembly: SuppressMessage("Style", "IDE0063:Use simple 'using' statement", Justification = "Better readability of scope", Scope = "member", Target = "~M:SimpleLogTest.TestImpl.SignalTestToContinue")]
