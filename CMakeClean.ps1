# clean_build.ps1
# CMake ビルドディレクトリを削除して再生成するスクリプト
# onedrive等でフォルダを同期した際にキャッシュ内のフォルダ構成と異なる場合、ビルドに失敗するため実行する。
# プロジェクトルートをスクリプトの場所に設定
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $projectRoot

# build ディレクトリの存在確認
if (Test-Path "$projectRoot\build") {
    Write-Host "Delete build Directory..."
    Remove-Item "$projectRoot\build" -Recurse -Force
}

# build ディレクトリを再作成
Write-Host "Make build Directory..."
New-Item -ItemType Directory -Path "$projectRoot\build" | Out-Null


Write-Host "clean_build End!"