param(
    [string]$TargetDir
)

$ErrorActionPreference = "Stop"

function Write-Info($msg) { Write-Host $msg -ForegroundColor Cyan }
function Write-Ok($msg) { Write-Host $msg -ForegroundColor Green }
function Write-Warn($msg) { Write-Host $msg -ForegroundColor Yellow }
function Write-Err($msg) { Write-Host $msg -ForegroundColor Red }

# Default target directory if not supplied
if (-not $TargetDir -or [string]::IsNullOrWhiteSpace($TargetDir)) {
    $scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
    $repoRoot = Resolve-Path (Join-Path $scriptDir "..")
    $TargetDir = Join-Path $repoRoot "resources\schematic xcore407i"
}

if (-not (Test-Path -LiteralPath $TargetDir -PathType Container)) {
    Write-Err "ERROR: target directory not found: $TargetDir"
    exit 2
}

$files = Get-ChildItem -LiteralPath $TargetDir -Filter "*.json" -File | Sort-Object Name
if (-not $files -or $files.Count -eq 0) {
    Write-Info "No .json files found under: $TargetDir"
    exit 0
}

[int]$total = 0
[int]$ok = 0
[int]$errCount = 0
[int]$warnCount = 0

Write-Info "Validating $($files.Count) files in: $TargetDir`n"

function Validate-Topology([object]$data) {
    $errors = New-Object System.Collections.Generic.List[string]
    $warnings = New-Object System.Collections.Generic.List[string]

    if (-not ($data.PSObject.Properties.Name -contains 'device' -or $data.PSObject.Properties.Name -contains 'subsystem')) {
        $warnings.Add("missing 'device' or 'subsystem' top-level key") | Out-Null
    }

    foreach ($pinsKey in @('pins','pins_1_to_150','mcu_power_pins')) {
        if ($data.PSObject.Properties.Name -contains $pinsKey) {
            $pins = $data.$pinsKey
            if (-not ($pins -is [System.Collections.IEnumerable])) {
                $errors.Add("'$pinsKey' must be a list") | Out-Null
            } else {
                $i = 0
                foreach ($p in $pins) {
                    if (-not ($p -is [pscustomobject])) {
                        $errors.Add("$pinsKey[$i] must be an object") | Out-Null
                        $i++
                        continue
                    }
                    $names = $p.PSObject.Properties.Name
                    if (-not ($names -contains 'name' -or $names -contains 'num' -or $names -contains 'pin' -or $names -contains 'header_pin')) {
                        $warnings.Add("$pinsKey[$i] missing typical pin identifier (name/num/pin/header_pin)") | Out-Null
                    }
                    $i++
                }
            }
        }
    }

    if ($data.PSObject.Properties.Name -contains 'components') {
        $comps = $data.components
        if (-not ($comps -is [System.Collections.IEnumerable])) {
            $errors.Add("'components' must be a list") | Out-Null
        } else {
            $i = 0
            foreach ($c in $comps) {
                if (-not ($c -is [pscustomobject])) { $errors.Add("components[$i] must be an object") | Out-Null; $i++; continue }
                $props = $c.PSObject.Properties.Name
                if (-not ($props -contains 'refdes')) { $warnings.Add("components[$i] missing 'refdes'") | Out-Null }
                if (-not ($props -contains 'value')) { $warnings.Add("components[$i] missing 'value'") | Out-Null }
                $i++
            }
        }
    }

    if ($data.PSObject.Properties.Name -contains 'nets') {
        $nets = $data.nets
        if (-not ($nets -is [System.Collections.IEnumerable])) {
            $errors.Add("'nets' must be a list") | Out-Null
        } else {
            $i = 0
            foreach ($n in $nets) {
                if (-not ($n -is [pscustomobject])) { $errors.Add("nets[$i] must be an object") | Out-Null; $i++; continue }
                $props = $n.PSObject.Properties.Name
                if (-not ($props -contains 'name')) { $warnings.Add("nets[$i] missing 'name'") | Out-Null }
                if (-not ($props -contains 'nodes' -or $props -contains 'from' -or $props -contains 'to')) {
                    $warnings.Add("nets[$i] missing 'nodes' (or from/to)") | Out-Null
                }
                $i++
            }
        }
    }

    return @($errors, $warnings)
}

foreach ($f in $files) {
    $total++
    $name = $f.Name
    try {
        $raw = Get-Content -LiteralPath $f.FullName -Raw -Encoding UTF8
        $data = $raw | ConvertFrom-Json -ErrorAction Stop
    }
    catch {
        $errCount++
        Write-Err "[ERROR] $name: invalid JSON -> $($_.Exception.Message)"
        continue
    }

    $result = Validate-Topology -data $data
    $errors = $result[0]
    $warnings = $result[1]

    if ($errors.Count -gt 0) {
        $errCount++
        Write-Err "[FAIL ] $name"
        foreach ($e in $errors) { Write-Host "  - error: $e" }
        foreach ($w in $warnings) { $warnCount++; Write-Warn "  - warn : $w" }
    }
    else {
        $ok++
        if ($warnings.Count -gt 0) {
            Write-Ok "[OK   ] $name -> OK (with warnings)"
        } else {
            Write-Ok "[OK   ] $name"
        }
        foreach ($w in $warnings) { $warnCount++; Write-Warn "  - warn : $w" }
    }
}

Write-Host "`nSummary:"
Write-Host ("  Total   : {0}" -f $total)
Write-Host ("  OK      : {0}" -f $ok)
Write-Host ("  Errors  : {0}" -f $errCount)
Write-Host ("  Warnings: {0}" -f $warnCount)

if ($errCount -gt 0) { exit 1 } else { exit 0 }
