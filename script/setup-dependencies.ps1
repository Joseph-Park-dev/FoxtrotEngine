$ErrorActionPreference = 'Stop'
$dependencyRoot = Join-Path (Split-Path $PSScriptRoot -Parent) 'third_party'
$dependencies = @(
    @('imgui', 'https://github.com/ocornut/imgui.git', 'dc5c6b3688d27dac69ae9e72f33c8f8fdb6cac69'),
    @('ImGuiFileDialog', 'https://github.com/aiekick/ImGuiFileDialog.git', 'd0e97b2adc3d3452d72c750c7305dc0291acd052'),
    @('json', 'https://github.com/nlohmann/json.git', 'bdbafc52c0ec52431e4e4aa9992775a734612d4b'),
    @('spine-runtimes', 'https://github.com/esotericsoftware/spine-runtimes.git', '393a6c2be0281486054b229de5b0f5b0b1dfb4d5'),
    @('magic_enum', 'https://github.com/Neargye/magic_enum.git', '1384769c66bd16ec9bb1353f45fe8ec8ccc12dbd')
)
New-Item -ItemType Directory -Path $dependencyRoot -Force | Out-Null
foreach ($dependency in $dependencies) {
    $destination = Join-Path $dependencyRoot $dependency[0]
    if (Test-Path -LiteralPath $destination) {
        $revision = & git -C $destination rev-parse HEAD
        if ($LASTEXITCODE -ne 0 -or $revision -ne $dependency[2]) {
            throw "Existing $destination does not match tested revision $($dependency[2]); preserve your changes and reconcile it manually."
        }
        Write-Output "Verified $($dependency[0]) $revision"
        continue
    }
    & git init $destination
    if ($LASTEXITCODE -ne 0) { throw 'git init failed' }
    & git -C $destination remote add origin $dependency[1]
    if ($LASTEXITCODE -ne 0) { throw 'git remote add failed' }
    & git -C $destination fetch --depth=1 origin $dependency[2]
    if ($LASTEXITCODE -ne 0) { throw "Fetch failed for $($dependency[0])" }
    & git -C $destination checkout --detach FETCH_HEAD
    if ($LASTEXITCODE -ne 0) { throw "Checkout failed for $($dependency[0])" }
}
