param(
    [string]$Qmake = "",
    [string]$Windeployqt = "",
    [string]$BuildTool = "",
    [string]$OutputDirectory = ""
)

$ErrorActionPreference = "Stop"
$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$projectFile = Join-Path $projectRoot "RigPlanIntegrator.pro"
$buildDirectory = Join-Path $projectRoot "build\windows-portable"

function Resolve-Executable([string]$specified, [string]$name) {
    if ($specified) {
        if (-not (Test-Path $specified)) {
            throw "Executável não encontrado: $specified"
        }
        return (Resolve-Path $specified).Path
    }

    $command = Get-Command $name -ErrorAction SilentlyContinue
    if ($command) {
        return $command.Source
    }
    return $null
}

$qmakePath = Resolve-Executable $Qmake "qmake.exe"
if (-not $qmakePath) {
    throw "qmake.exe não encontrado. Instale o Qt 6 para Windows e informe -Qmake ou adicione o diretório bin ao PATH."
}

$qtBin = Split-Path $qmakePath -Parent
$windeployqtPath = Resolve-Executable $Windeployqt "windeployqt.exe"
if (-not $windeployqtPath) {
    $candidate = Join-Path $qtBin "windeployqt.exe"
    if (Test-Path $candidate) {
        $windeployqtPath = $candidate
    } else {
        throw "windeployqt.exe não encontrado. Informe -Windeployqt ou use o diretório bin do Qt no PATH."
    }
}

if (-not $BuildTool) {
    foreach ($toolName in @("nmake.exe", "mingw32-make.exe", "make.exe")) {
        $command = Get-Command $toolName -ErrorAction SilentlyContinue
        if ($command) {
            $BuildTool = $command.Source
            break
        }
    }
}
if (-not $BuildTool -or -not (Test-Path $BuildTool)) {
    throw "Ferramenta de build não encontrada. Use um Developer PowerShell do Visual Studio para MSVC ou adicione mingw32-make.exe ao PATH."
}

if (-not $OutputDirectory) {
    $OutputDirectory = Join-Path $projectRoot "dist\RigPlanIntegrator-Windows"
}
$OutputDirectory = [System.IO.Path]::GetFullPath($OutputDirectory)

if (Test-Path $buildDirectory) { Remove-Item $buildDirectory -Recurse -Force }
if (Test-Path $OutputDirectory) { Remove-Item $OutputDirectory -Recurse -Force }
New-Item -ItemType Directory -Path $buildDirectory -Force | Out-Null
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null

Push-Location $buildDirectory
try {
    & $qmakePath $projectFile "CONFIG+=release"
    if ($LASTEXITCODE -ne 0) { throw "qmake falhou com código $LASTEXITCODE." }

    & $BuildTool
    if ($LASTEXITCODE -ne 0) { throw "Compilação falhou com código $LASTEXITCODE." }
} finally {
    Pop-Location
}

$executable = Get-ChildItem -Path $buildDirectory -Filter "RigPlanIntegrator.exe" -File -Recurse |
    Select-Object -First 1 -ExpandProperty FullName
if (-not $executable) {
    throw "O build terminou, mas RigPlanIntegrator.exe não foi encontrado em $buildDirectory."
}

& $windeployqtPath "--release" "--compiler-runtime" "--no-translations" "--dir" $OutputDirectory $executable
if ($LASTEXITCODE -ne 0) { throw "windeployqt falhou com código $LASTEXITCODE." }

New-Item -ItemType Directory -Path (Join-Path $OutputDirectory "portable_data") -Force | Out-Null
Copy-Item (Join-Path $projectRoot "WINDOWS_PORTABLE.md") $OutputDirectory

$zipPath = "$OutputDirectory.zip"
if (Test-Path $zipPath) { Remove-Item $zipPath -Force }
# Include the package directory itself in the ZIP, matching the v1.0.0 portable layout.
Compress-Archive -Path $OutputDirectory -DestinationPath $zipPath -CompressionLevel Optimal

Write-Host "Versão portátil criada em: $OutputDirectory"
Write-Host "Arquivo ZIP criado em: $zipPath"