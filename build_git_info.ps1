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
$versionTag = Get-GitValue -GitArguments @("describe", "--tags", "--abbrev=0", "HEAD") -Fallback ""
$versionParts = @(0, 0, 0, 0)
$versionString = "0.0.0"
if ($versionTag -match '^v?(\d+)\.(\d+)(?:\.(\d+))?(?:\.(\d+))?(?:[-+].+)?$') {
    for ($index = 0; $index -lt 4; ++$index) {
        if ($Matches[$index + 1]) {
            $versionParts[$index] = [uint16]::Parse($Matches[$index + 1])
        }
    }
    $versionString = $versionParts[0..2] -join "."
    if ($Matches[4]) {
        $versionString += ".$($versionParts[3])"
    }
}
elseif ($versionTag) {
    Write-Warning "Tag '$versionTag' is not a version tag; using 0.0.0 for the version resource."
}
$content = @(
    "#pragma once"
    "#define BRWATCH_GIT_HASH $(ConvertTo-Json -InputObject $gitHash -Compress)"
    "#define BRWATCH_GIT_TAG $(ConvertTo-Json -InputObject $gitTag -Compress)"
    "#define BRWATCH_PRODUCTVERSION $($versionParts -join ',')"
    "#define BRWATCH_PRODUCTVERSION_STR $(ConvertTo-Json -InputObject $versionString -Compress)"
    ""
) -join "`n"

$OutputPath = [System.IO.Path]::GetFullPath($OutputPath)
if (-not (Test-Path -LiteralPath $OutputPath) -or [System.IO.File]::ReadAllText($OutputPath) -cne $content) {
    [System.IO.Directory]::CreateDirectory([System.IO.Path]::GetDirectoryName($OutputPath)) | Out-Null
    [System.IO.File]::WriteAllText($OutputPath, $content, [System.Text.UTF8Encoding]::new($false))
}