param(
    [string]$OutputPath = (Join-Path $PSScriptRoot "build/git_info.h")
)

$ErrorActionPreference = "Stop"

function Get-GitValue {
    param([string[]]$GitArguments, [string]$Fallback)

    try {
        $value = & git -C $PSScriptRoot @GitArguments 2>$null
        if ($LASTEXITCODE -eq 0 -and $value) {
            return ($value -join "`n").Trim()
        }
    }
    catch {
        return $Fallback
    }
    return $Fallback
}

$gitHash = Get-GitValue -GitArguments @("rev-parse", "--short=12", "HEAD") -Fallback "unknown"
$gitTag = Get-GitValue -GitArguments @("describe", "--tags", "--exact-match", "HEAD") -Fallback "untagged"
$content = @(
    "#pragma once"
    "#define BRWATCH_GIT_HASH $(ConvertTo-Json -InputObject $gitHash -Compress)"
    "#define BRWATCH_GIT_TAG $(ConvertTo-Json -InputObject $gitTag -Compress)"
    ""
) -join "`n"

$OutputPath = [System.IO.Path]::GetFullPath($OutputPath)
if (-not (Test-Path -LiteralPath $OutputPath) -or [System.IO.File]::ReadAllText($OutputPath) -cne $content) {
    [System.IO.Directory]::CreateDirectory([System.IO.Path]::GetDirectoryName($OutputPath)) | Out-Null
    [System.IO.File]::WriteAllText($OutputPath, $content, [System.Text.UTF8Encoding]::new($false))
}