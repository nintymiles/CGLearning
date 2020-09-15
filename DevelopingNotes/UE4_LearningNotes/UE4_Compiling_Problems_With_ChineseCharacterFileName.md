# UE4打包时遭遇编译错误“C1083"(winodws环境)

在打包UE4的样例项目时，遇到C++编译问题。具体为无法找到预编译文件“SharedPCH.Core.h.pch”文，提示“c1xx"错误。在经历折腾之后，发现问题出在UE4打包过程所生成中间文件目录中包含中文名（项目文件夹名称和项目名称都会导致中间文件结构中出现中文名的情形）。

这意味着UE4直到4.25对于中文目录的支持都是有问题的（当然可能是所依赖底层编译器导致的）。


```
BuildMode.Build: Building 山脉景观...
BuildMode.OutputToolchainInfo: Using Visual Studio 2019 14.27.29111 toolchain (C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.27.29110) and Windows 10.0.18362.0 SDK (C:\Program Files (x86)\Windows Kits\10).
BuildMode.OutputToolchainInfo: [Upgrade]
BuildMode.OutputToolchainInfo: [Upgrade] Using backward-compatible build settings. The latest version of UE4 sets the following values by default, which may require code changes:
BuildMode.OutputToolchainInfo: [Upgrade]     bLegacyPublicIncludePaths = false                 => Omits subfolders from public include paths to reduce compiler command line length. (Previously: true).
BuildMode.OutputToolchainInfo: [Upgrade]     ShadowVariableWarningLevel = WarningLevel.Error   => Treats shadowed variable warnings as errors. (Previously: WarningLevel.Warning).
BuildMode.OutputToolchainInfo: [Upgrade]     PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs   => Set in build.cs files to enables IWYU-style PCH model. See https://docs.unrealengine.com/en-US/Programming/BuildTools/UnrealBuildTool/IWYU/index.html. (Previously: PCHUsageMode.UseSharedPCHs).
BuildMode.OutputToolchainInfo: [Upgrade] Suppress this message by setting 'DefaultBuildSettings = BuildSettingsVersion.V2;' in 山脉景观.Target.cs, and explicitly overriding settings that differ from the new defaults.
BuildMode.OutputToolchainInfo: [Upgrade]
ParallelExecutor.ExecuteActions: Building 4 actions with 8 processes...
ParallelExecutor.ExecuteActions:   [1/4] SharedPCH.Core.cpp
ParallelExecutor.ExecuteActions:   c1xx: fatal error C1083: Cannot open compiler intermediate file: 'C:\Users\xxxx\Documents\Unreal Projects\MountainView\Intermediate\Build\Win64\山脉景观\Development\Core\SharedPCH.Core.h.pch': No such file or directy
UnrealBuildTool.Main: CompilationResultException: Error: OtherCompilationError
UnrealBuildTool.Main:    在 UnrealBuildTool.ActionGraph.ExecuteActions(BuildConfiguration BuildConfiguration, List`1 ActionsToExecute) 位置 D:\Build\++UE4+Licensee\Sync\Engine\Saved\CsTools\Engine\Source\Programs\UnrealBuildTool\System\ActionGraph.cs:行号 242
UnrealBuildTool.Main:    在 UnrealBuildTool.BuildMode.Build(List`1 TargetDescriptors, BuildConfiguration BuildConfiguration, ISourceFileWorkingSet WorkingSet, BuildOptions Options, FileReference WriteOutdatedActionsFile) 位置 D:\Build\++UE4+Licensee\Sync\Engine\Saved\CsTools\Engine\Source\Programs\UnrealBuildTool\Modes\BuildMode.cs:行号 372
UnrealBuildTool.Main:    在 UnrealBuildTool.BuildMode.Execute(CommandLineArguments Arguments) 位置 D:\Build\++UE4+Licensee\Sync\Engine\Saved\CsTools\Engine\Source\Programs\UnrealBuildTool\Modes\BuildMode.cs:行号 219
UnrealBuildTool.Main:    在 UnrealBuildTool.UnrealBuildTool.Main(String[] ArgumentsArray) 位置 D:\Build\++UE4+Licensee\Sync\Engine\Saved\CsTools\Engine\Source\Programs\UnrealBuildTool\UnrealBuildTool.cs:行号 550
Timeline.Print: Timeline:
Timeline.Print: 
Timeline.Print: [ 0.000]
Timeline.Print: [ 0.000](+0.041) <unknown>
Timeline.Print: [ 0.041](+0.003) FileMetadataPrefetch.QueueEngineDirectory()
Timeline.Print: [ 0.044](+0.502) XmlConfig.ReadConfigFiles()
Timeline.Print: [ 0.546](+0.000) SingleInstanceMutex.Acquire()
Timeline.Print: [ 0.546](+0.282) UEBuildPlatform.RegisterPlatforms()
Timeline.Print:   0.548          [ 0.001](+0.137) Initializing InstalledPlatformInfo
Timeline.Print:   0.685          [ 0.138](+0.000) Querying types
Timeline.Print:   0.689          [ 0.142](+0.006) MacPlatformFactory
Timeline.Print:   0.695          [ 0.148](+0.001) TVOSPlatformFactory
Timeline.Print:   0.697          [ 0.150](+0.001) AndroidPlatformFactory
Timeline.Print:   0.699          [ 0.152](+0.001) HoloLensPlatformFactory
Timeline.Print:   0.700          [ 0.154](+0.006) IOSPlatformFactory
Timeline.Print:   0.708          [ 0.162](+0.002) LinuxPlatformFactory
Timeline.Print:   0.711          [ 0.164](+0.000) LuminPlatformFactory
Timeline.Print:   0.712          [ 0.165](+0.116) WindowsPlatformFactory
Timeline.Print: [ 0.829](+0.068) <unknown>
Timeline.Print: [ 0.898](+0.044) TargetDescriptor.ParseCommandLine()
Timeline.Print: [ 0.942](+0.038) <unknown>
Timeline.Print: [ 0.981](+1.204) UEBuildTarget.Create()
Timeline.Print:   0.988          [ 0.007](+0.347) RulesCompiler.CreateTargetRulesAssembly()
Timeline.Print:   0.988            0.007          [ 0.000](+0.162) <unknown>
Timeline.Print:   1.150            0.169          [ 0.162](+0.076) Finding engine modules
Timeline.Print:   1.227            0.246          [ 0.239](+0.008) Finding plugin modules
Timeline.Print:   1.235            0.254          [ 0.247](+0.052) <unknown>
Timeline.Print:   1.288            0.306          [ 0.299](+0.016) Finding program modules
Timeline.Print:   1.304            0.323          [ 0.315](+0.007) Finding program targets
Timeline.Print:   1.311            0.330          [ 0.322](+0.024) <unknown>
Timeline.Print:   1.336          [ 0.355](+0.153) RulesAssembly.CreateTargetRules()
Timeline.Print:   1.489          [ 0.508](+0.039) <unknown>
Timeline.Print:   1.528          [ 0.547](+0.102) UEBuildTarget constructor
Timeline.Print:   1.536            0.555          [ 0.007](+0.091) Reading source file metadata cache
Timeline.Print:   1.631          [ 0.650](+0.553) UEBuildTarget.PreBuildSetup()
Timeline.Print: [ 2.191](+0.672) UEBuildTarget.Build()
Timeline.Print:   2.191          [ 0.000](+0.068) <unknown>
Timeline.Print:   2.260          [ 0.068](+0.044) ExternalExecution.SetupUObjectModules()
Timeline.Print:   2.321          [ 0.129](+0.001) ExternalExecution.UpdateDirectoryTimestamps()
Timeline.Print:   2.338          [ 0.146](+0.430) UEBuildBinary.Build()
Timeline.Print:   2.769          [ 0.577](+0.095) <unknown>
Timeline.Print: [ 2.866](+0.002) ActionGraph.CheckPathLengths
Timeline.Print: [ 2.868](+0.022) <unknown>
Timeline.Print: [ 2.890](+0.003) Reading dependency cache
Timeline.Print: [ 2.894](+0.007) Reading action history
Timeline.Print: [ 2.902](+0.014) ActionGraph.GetActionsToExecute()
Timeline.Print:   2.903          [ 0.000](+0.002) Prefetching include dependencies
Timeline.Print:   2.906          [ 0.003](+0.010) Cache outdated actions
Timeline.Print: [ 2.922](+0.428) ActionGraph.ExecuteActions()
Timeline.Print: [ 3.351](+0.031) <unknown>
Timeline.Print: [ 3.382](+0.000) FileMetadataPrefetch.Stop()
Timeline.Print: [ 3.386]

```