# Check if running as administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Host "Please run as Administrator"
    exit 1
}

# Create build directory if it doesn't exist
New-Item -ItemType Directory -Force -Path "build"

# Build the programs
Write-Host "Building programs..."
Set-Location build
cmake ..
cmake --build .

# Install the programs
Write-Host "Installing programs..."
$installPath = "$env:ProgramFiles\ProcessManager"
New-Item -ItemType Directory -Force -Path $installPath
Copy-Item "procmanager.exe" $installPath
Copy-Item "procmanager-gui.exe" $installPath

# Add to PATH
$currentPath = [Environment]::GetEnvironmentVariable("Path", "Machine")
if (-not $currentPath.Contains($installPath)) {
    [Environment]::SetEnvironmentVariable("Path", "$currentPath;$installPath", "Machine")
}

# Install PowerShell help files
Write-Host "Installing PowerShell help files..."
$helpPath = "$env:ProgramFiles\WindowsPowerShell\Modules\ProcessManager"
New-Item -ItemType Directory -Force -Path $helpPath
Copy-Item "..\help\*.ps1" $helpPath

# Create module manifest
$manifestPath = "$helpPath\ProcessManager.psd1"
@"
@{
    ModuleVersion = '1.0'
    GUID = '$(New-Guid)'
    Author = 'Your Name'
    CompanyName = 'Your Company'
    Copyright = '(c) 2024. All rights reserved.'
    Description = 'Process Manager Module'
    PowerShellVersion = '5.1'
    FunctionsToExport = @('procmanager', 'procmanager-gui')
    CmdletsToExport = @()
    VariablesToExport = '*'
    AliasesToExport = @()
    PrivateData = @{
        PSData = @{
            Tags = @('Process', 'Management')
            ProjectUri = 'https://github.com/yourusername/process-manager'
        }
    }
}
"@ | Out-File -FilePath $manifestPath -Encoding UTF8

Write-Host "Installation complete!"
Write-Host "You can now use 'Get-Help procmanager' or 'Get-Help procmanager-gui' to view the help files." 