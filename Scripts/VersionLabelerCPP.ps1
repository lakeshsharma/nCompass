#####################################################################################################
#
# Name:  VersionLabelerCPP.ps1
# Author:  Larry Paseornek
# Snapins Used:  none
# Purpose:  To edit Visual Studio .RC files to the ccnet version using the environmental var  
# Execution:  Run this script from any directory.  It will recursively update the versions
# Last Edit:  2011-07-22 Alan Sheldon -- Only update version information.
#
#######################################################################################################
param($FileName, [string]$OverrideVersion="")
#cd "D:\Build\VertexDriverSimulator\Trunk"
#$FileName = "Simulator.rc"
$CCVer = $env:CCNetLabel;
if ($OverrideVersion -ne "")
{
    $CCVer = $OverrideVersion;
}

$Version = $($CCVer).replace(".",",")
$VerionDot = $CCVer
#$Version = "1,1,1,1"
foreach ($file in Get-ChildItem -Recurse -Include $FileName)
{
	$TempFile = $file.FullName + ".tmp"
	Get-Content $file.FullName -Encoding ASCII | 
		%{$_ -creplace '(FILEVERSION\s+)[0-9]+(\,([0-9]+|\*)){3}', ('${1}' + $Version) } |
		%{$_ -creplace '(FileVersion\D*)[0-9]+(\.([0-9]+|\*)){3}', ('${1}' + $VerionDot) } |
		%{$_ -creplace '(PRODUCTVERSION\s+)[0-9]+(\,([0-9]+|\*)){3}', ('${1}' + $Version) } |
		%{$_ -creplace '(ProductVersion\D*)[0-9]+(\.([0-9]+|\*)){3}', ('${1}' + $VerionDot) } | 
		set-content -Encoding ASCII $TempFile
	Move-Item $TempFile $file.FullName -Force
}