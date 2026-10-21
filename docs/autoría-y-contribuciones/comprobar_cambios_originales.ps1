# Comprueba si el repositorio original (K0NL/B2recomp) ha incorporado cambios equivalentes a los de este proyecto.
# Uso:  pwsh -File comprobar_cambios_originales.ps1      (necesita git y conexión)
$tmp = Join-Path $env:TEMP 'b2recomp-original'
if (-not (Test-Path $tmp)) { git clone -q https://github.com/K0NL/B2recomp.git $tmp }
Set-Location $tmp
git fetch -q origin
"Ejecutado: " + (Get-Date).ToUniversalTime().ToString('s') + "Z"
"Estado comprobado: origin/main " + (git rev-parse --short origin/main) + " " + (git log -1 --format=%aI origin/main) + " | origin/linux " + (git rev-parse --short origin/linux)
"--- Identificadores propios (0 = no aparecen en el original, ni en main ni en linux):"
foreach ($p in 'swapchain_stale', 'aout_of', 'AOut ', 'menu_vk', 'recomps_cheat', 'cheats_patch', 'menu_captures_input') {
    "{0,-22} main={1} linux={2}" -f $p, (@(git grep -l -F $p origin/main -- . 2>$null).Count), (@(git grep -l -F $p origin/linux -- . 2>$null).Count)
}
"--- nnaudio.c: diferencias entre v0.2.8 (e95cbeb, base de nuestro PR) y main actual:"
$d = git diff --stat e95cbeb origin/main -- runtime/nnaudio.c
if ($d) { $d } else { "(ninguna: el archivo es idéntico)" }
"--- Regla de volumen por nombre del ejecutable, en main actual:"
git grep -n 'ssbu_native.exe' origin/main -- runtime/nnaudio.c | ForEach-Object { $_.Substring(0, [Math]::Min(200, $_.Length)) }
"--- Recreación de la swapchain ante VK_SUBOPTIMAL_KHR en main actual:"
git grep -n 'VK_ERROR_OUT_OF_DATE_KHR || r == VK_SUBOPTIMAL_KHR' origin/main -- runtime | ForEach-Object { $_.Substring(0, [Math]::Min(170, $_.Length)) }
"--- Commits del original posteriores a v0.2.8 que tocan audio, nvn_vk.c, display.c, nnhid.c, game_shim.c o setup.py:"
git log --format='%h %aI %s' e95cbeb..origin/main -- runtime/nnaudio.c runtime/nvn_vk.c runtime/display.c runtime/nnhid.c runtime/game_shim.c scripts/setup.py
