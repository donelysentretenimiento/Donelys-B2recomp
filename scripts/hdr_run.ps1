# Test run of the real game path (as play.ps1) with an isolated copy of the save, a time limit and the log in build\hdr_runs\<name>\.
# Usage: .\scripts\hdr_run.ps1 <name> [seconds] [-Hdr off|auto|hdr10|scrgb] [-Show] [-Validate]
# Does not touch build\play\userdata (the player's saves): the run's saves are a copy of build\saves\after-prologue.
param([Parameter(Mandatory)][string]$Name, [int]$Seconds = 60, [string]$Hdr = '', [switch]$Show, [switch]$Validate, [string]$Exe = '')
$root = Split-Path $PSScriptRoot
$run = Join-Path $root "build\hdr_runs\$Name"
New-Item -ItemType Directory -Force "$run\userdata" | Out-Null
if (-not (Test-Path "$run\userdata\save") -and (Test-Path "$root\build\saves\after-prologue\save")) { Copy-Item -Recurse -Force "$root\build\saves\after-prologue\*" "$run\userdata\" }
foreach ($v in 'SSBU_SEQ', 'SSBU_SCRIPT', 'SSBU_PRESS', 'SSBU_SHOT', 'SSBU_SHOT_EVERY', 'SSBU_SHOT_DIR', 'SSBU_PROF15', 'SSBU_HITCH', 'SSBU_TRACE') { [Environment]::SetEnvironmentVariable($v, $null) }
$env:SSBU_ROMFS = Join-Path $root 'targets\romfs'
$env:SSBU_USERDATA = "$run\userdata"
$env:SSBU_SHOW = if ($Show) { '1' } else { '0' }; $env:SSBU_VOLUME = if ($Show) { '100' } else { '0' }
$env:SSBU_FOCUS_NOTIFY = '0'
if (-not $env:SSBU_GRAPHICS_CONFIG) { $env:SSBU_GRAPHICS_CONFIG = Join-Path $root 'config\graphics.ini' }      # a test can point it at a copy
$env:SSBU_CONFIG = Join-Path $root 'config\controllers.ini'
$env:SSBU_HDR = if ($Hdr) { $Hdr } else { $null }
$env:SSBU_VK_VALIDATE = if ($Validate) { '1' } else { $null }
$fonts = Join-Path $root 'targets\sysdata\fonts'; if (Test-Path $fonts) { $env:SSBU_FONTS = $fonts }
$exe = if ($Exe) { $Exe } else { Join-Path $root 'build\game_native.exe' }
$log = "$run\stdout.txt"
$psi = New-Object Diagnostics.ProcessStartInfo 'cmd.exe', "/c `"`"$exe`" `"$(Join-Path $root 'targets\exefs\main.elf')`" $Seconds > `"$log`" 2>&1`""
$psi.WorkingDirectory = Join-Path $root 'build'; $psi.UseShellExecute = $false; $psi.CreateNoWindow = $true
$p = [Diagnostics.Process]::Start($psi)
$p.WaitForExit(($Seconds + 90) * 1000) | Out-Null
"exit code $($p.ExitCode); log: $log"
