#
# Argument initialization
#

$nextarg = "none"
$DebugPort = "unassigned"
$targetcomputer = "."
$VMName = ""
$VMGuid = ""
$AutoAssign = "false"
$DebugOff = "false"

function funHelp()
{
$helpText=@"

DESCRIPTION:
NAME: kdnetdebugvm.ps1
Displays (and optionally sets) the port used to network debug a VM.

PARAMETERS: 
-computerName Specifies the name of the computer on which to run the script
-help         Displays help
-vmname       (optional) Name of the VM of interest
-vmguid       (optional) GUID of the VM of interest
-port         (optional) Network port to use for debugging
-debugoff
-autoassign

Either vmname or vmguid must be specified to identify the VM, but not both.
Note that vmname may not uniquely identify the VM, but vmguid does.

SYNTAX:
kdnetdebugvm.ps1 [-computerName targetcomputer] [-vmname NameOfVM] [-vmguid GuidOfVM] [-port PortNumber]

"@
$helpText
exit
}


foreach ($argument in $args)
{
    # parse commands with no following arguments
    switch ($argument)
    {
        "?"     {funHelp}
        "help"  {funHelp}
        "-help" {funHelp}
        "/?"    {funHelp}
        "-?"    {funHelp}
        "autoassign"    {$AutoAssign = "true"}
        "-autoassign"   {$AutoAssign = "true"}
        "/autoassign"   {$AutoAssign = "true"}
        "debugoff"        {$DebugOff = "true"}
        "-debugoff"       {$DebugOff = "true"}
        "/debugoff"       {$DebugOff = "true"}
        default {}
    }

    # parse values that followed a switch

    switch ($nextarg)
    {
        "vmname"        {$VMName = $argument}
        "-vmname"       {$VMName = $argument}
        "/vmname"       {$VMName = $argument}
        "vmguid"        {$VMGuid = $argument}
        "-vmguid"       {$VMGuid = $argument}
        "/vmguid"       {$VMGuid = $argument}
        "port"          {$DebugPort = $argument}
        "-port"         {$DebugPort = $argument}
        "/port"         {$DebugPort = $argument}
        "computername"  {$targetcomputer = $argument}
        "-computername" {$targetcomputer = $argument}
        "/computername" {$targetcomputer = $argument}
        default         {}
    }

    $nextarg = $argument
}

if (($VMName -eq "") -and ($VMGuid -eq ""))
{
    funHelp
}

if (($VMName -ne "") -and ($VMGuid -ne ""))
{
    funHelp
}

$ns = "root\virtualization\v2"
$VMWPName = "$env:windir\system32\vmwp.exe"

#Get a VMManagementService object
$VMManagementService = gwmi -class "Msvm_VirtualSystemManagementService" -namespace $ns -computername $targetcomputer

#Get the VM object that we want to modify
if ($VMName -ne "")
{
    $VM = Get-VM -computername $targetcomputer -VMName $VMName
}

if ($VMGuid -ne "")
{
    $VM = Get-VM -computername $targetcomputer -Id $VMGuid
}

#Get the VirtualSystemGlobalSettingData of the VM we want to modify
$VMSystemGlobalSettingData = gwmi -namespace $ns -computername $targetcomputer -class Msvm_VirtualSystemSettingData | ? { $_.ConfigurationID -eq $VM.Id }

# Set a new debugport
if ($DebugPort -ne "unassigned")
{
    #Change the ElementName property
    $VMSystemGlobalSettingData.DebugPort = $DebugPort
    $VMSystemGlobalSettingData.DebugPortEnabled = 1

    $VMManagementService.ModifySystemSettings($VMSystemGlobalSettingData.GetText(1))
    $FWRuleName = "SynthDebugInboundRule-$DebugPort"
    New-NetFirewallRule -DisplayName $FWRuleName -Program $VMWPName -Protocol UDP -Action Allow -Direction Inbound -LocalPort $DebugPort
}

# Enable auto assigned debug ports
if ($AutoAssign -ne "false")
{
    #Change the ElementName property
    $VMSystemGlobalSettingData.DebugPortEnabled = 2
    $VMManagementService.ModifySystemSettings($VMSystemGlobalSettingData.GetText(1))
    Write-Host -Foreground Yellow "Firewall Ports for autoassign mode can be opened only after the VM is started."
}

# Turn off debugging
if ($DebugOff -ne "false")
{
    $DebugPort = $VMSystemGlobalSettingData.DebugPort
    #Change the ElementName property
    $VMSystemGlobalSettingData.DebugPortEnabled = 0
    $VMSystemGlobalSettingData.DebugPort = 0
    $VMManagementService.ModifySystemSettings($VMSystemGlobalSettingData.GetText(1))
    # May throw an exception if the rule did not exist already.
    # If two rules exist with the same name, both will be deleted.
    if ($DebugPort -ne 0)
    {
        $FWRuleName = "SynthDebugInboundRule-$DebugPort"
        Remove-NetFirewallRule -DisplayName $FWRuleName
    }
}

$VMSystemGlobalSettingData

exit

# SIG # Begin signature block
# MIIiPAYJKoZIhvcNAQcCoIIiLTCCIikCAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCDbHDLYuqp9Du8W
# ZSYoLwjEyhkYywkzwemxEtpUqBqajKCCC38wggUHMIID76ADAgECAhMzAAABtGsb
# Q7hXc1IAAAAAAAG0MA0GCSqGSIb3DQEBCwUAMH4xCzAJBgNVBAYTAlVTMRMwEQYD
# VQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNy
# b3NvZnQgQ29ycG9yYXRpb24xKDAmBgNVBAMTH01pY3Jvc29mdCBDb2RlIFNpZ25p
# bmcgUENBIDIwMTAwHhcNMTcwNzE4MTc1MDQyWhcNMTgwNzEwMTc1MDQyWjB/MQsw
# CQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
# ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSkwJwYDVQQDEyBNaWNy
# b3NvZnQgV2luZG93cyBLaXRzIFB1Ymxpc2hlcjCCASIwDQYJKoZIhvcNAQEBBQAD
# ggEPADCCAQoCggEBALoNUC4sIGNyRsCwy4Hz1lNmFXull+eDYoV8icPzo7fp48hp
# WfrMVqFCARGABNkFu7NG+9vJw2ZxMGjOF/PPqLUnYr9wOuEqozMtkvKpR8mU0he4
# gzoPoYfVl3D2M/UPVyvfkeLiIIZ0KqrGkjE5Nib/Xcngu6gLGXj9dUXHA8dA40Hg
# BaIls59ygmky4zuMiH59X3ZFQ5PFuT7CLZLt0IDuHUUL6Cv2JgicUoerLEdWrTq9
# vtEtPGlM9ygDgOpM00ws9yRliMxiUhuCQbwJSNKhkDs/rNSrB6KCpKC8MWdIs0Wk
# Oac3y/1LRJB0T/XUvCUsq3Z89eBrAgs6Izqu5QsCAwEAAaOCAXswggF3MB8GA1Ud
# JQQYMBYGCisGAQQBgjcKAxQGCCsGAQUFBwMDMB0GA1UdDgQWBBRn2V9L5fyECUI1
# 7j47Ui+9hpG8pDBSBgNVHREESzBJpEcwRTENMAsGA1UECxMETU9QUjE0MDIGA1UE
# BRMrMjI5OTAzK2ZkNmI5ZTVkLWViNzMtNDE4OS1hYmMyLWY3Y2FjZGEzODFhYzAf
# BgNVHSMEGDAWgBTm/F97uyIAWORyTrX0IXQjMubvrDBWBgNVHR8ETzBNMEugSaBH
# hkVodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20vcGtpL2NybC9wcm9kdWN0cy9NaWND
# b2RTaWdQQ0FfMjAxMC0wNy0wNi5jcmwwWgYIKwYBBQUHAQEETjBMMEoGCCsGAQUF
# BzAChj5odHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL01pY0NvZFNp
# Z1BDQV8yMDEwLTA3LTA2LmNydDAMBgNVHRMBAf8EAjAAMA0GCSqGSIb3DQEBCwUA
# A4IBAQCir8BXOUv+Q73WyLmTPSVPGlvkmzwOjxOXAiyZAWIdvY1Bz0kGpMv4UMBw
# 6LCcY/niXi2Tq0ji3rTYzs+QSg+hKgAJp6KP8lIM9L5uyPa9jF0F6TK15mH58XSt
# 7ed6evQ+rNqFCcfFacX/KHfEq/SWJ0yiUHhKBzKGv940rzQuzN30TuCYxzKR85hl
# fGn3xz1EN3Knbkm7j5S9HIaaz1i4sDBp1V6qLY2wCgNdaTnyC8vjWfya+91wigW8
# i5EyWwmCgKqP828UHGWUO5J1WWzKZSOmQmpZo0sDRts7M4J8X8laqtbBi2qN5Om4
# PDTVwzIiL04c5IWK3sq4srU+WKBIMIIGcDCCBFigAwIBAgIKYQxSTAAAAAAAAzAN
# BgkqhkiG9w0BAQsFADCBiDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0
# b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3Jh
# dGlvbjEyMDAGA1UEAxMpTWljcm9zb2Z0IFJvb3QgQ2VydGlmaWNhdGUgQXV0aG9y
# aXR5IDIwMTAwHhcNMTAwNzA2MjA0MDE3WhcNMjUwNzA2MjA1MDE3WjB+MQswCQYD
# VQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEe
# MBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSgwJgYDVQQDEx9NaWNyb3Nv
# ZnQgQ29kZSBTaWduaW5nIFBDQSAyMDEwMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A
# MIIBCgKCAQEA6Q5kUHlntcTj/QkATJ6UrPdWaOpE2M/FWE+ppXZ8bUW60zmStKQe
# +fllguQX0o/9RJwI6GWTzixVhL99COMuK6hBKxi3oktuSUxrFQfe0dLCiR5xlM21
# f0u0rwjYzIjWaxeUOpPOJj/s5v40mFfVHV1J9rIqLtWFu1k/+JC0K4N0yiuzO0bj
# 8EZJwRdmVMkcvR3EVWJXcvhnuSUgNN5dpqWVXqsogM3Vsp7lA7Vj07IUyMHIiiYK
# WX8H7P8O7YASNUwSpr5SW/Wm2uCLC0h31oVH1RC5xuiq7otqLQVcYMa0KlucIxxf
# ReMaFB5vN8sZM4BqiU2jamZjeJPVMM+VHwIDAQABo4IB4zCCAd8wEAYJKwYBBAGC
# NxUBBAMCAQAwHQYDVR0OBBYEFOb8X3u7IgBY5HJOtfQhdCMy5u+sMBkGCSsGAQQB
# gjcUAgQMHgoAUwB1AGIAQwBBMAsGA1UdDwQEAwIBhjAPBgNVHRMBAf8EBTADAQH/
# MB8GA1UdIwQYMBaAFNX2VsuP6KJcYmjRPZSQW9fOmhjEMFYGA1UdHwRPME0wS6BJ
# oEeGRWh0dHA6Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvY3JsL3Byb2R1Y3RzL01p
# Y1Jvb0NlckF1dF8yMDEwLTA2LTIzLmNybDBaBggrBgEFBQcBAQROMEwwSgYIKwYB
# BQUHMAKGPmh0dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9wa2kvY2VydHMvTWljUm9v
# Q2VyQXV0XzIwMTAtMDYtMjMuY3J0MIGdBgNVHSAEgZUwgZIwgY8GCSsGAQQBgjcu
# AzCBgTA9BggrBgEFBQcCARYxaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL1BLSS9k
# b2NzL0NQUy9kZWZhdWx0Lmh0bTBABggrBgEFBQcCAjA0HjIgHQBMAGUAZwBhAGwA
# XwBQAG8AbABpAGMAeQBfAFMAdABhAHQAZQBtAGUAbgB0AC4gHTANBgkqhkiG9w0B
# AQsFAAOCAgEAGnTvV08pe8QWhXi4UNMi/AmdrIKX+DT/KiyXlRLl5L/Pv5PI4zSp
# 24G43B4AvtI1b6/lf3mVd+UC1PHr2M1OHhthosJaIxrwjKhiUUVnCOM/PB6T+DCF
# F8g5QKbXDrMhKeWloWmMIpPMdJjnoUdD8lOswA8waX/+0iUgbW9h098H1dlyACxp
# hnY9UdumOUjJN2FtB91TGcun1mHCv+KDqw/ga5uV1n0oUbCJSlGkmmzItx9KGg5p
# qdfcwX7RSXCqtq27ckdjF/qm1qKmhuyoEESbY7ayaYkGx0aGehg/6MUdIdV7+QIj
# LcVBy78dTMgW77Gcf/wiS0mKbhXjpn92W9FTeZGFndXS2z1zNfM8rlSyUkdqwKoT
# ldKOEdqZZ14yjPs3hdHcdYWch8ZaV4XCv90Nj4ybLeu07s8n07VeafqkFgQBpyRn
# c89NT7beBVaXevfpUk30dwVPhcbYC/GO7UIJ0Q124yNWeCImNr7KsYxuqh3khdpH
# M2KPpMmRM19xHkCvmGXJIuhCISWKHC1g2TeJQYkqFg/XYTyUaGBS79ZHmaCAQO4V
# gXc+nOBTGBpQHTiVmx5mMxMnORd4hzbOTsNfsvU9R1O24OXbC2E9KteSLM43Wj5A
# QjGkHxAIwlacvyRdUQKdannSF9PawZSOB3slcUSrBmrm1MbfI5qWdcUxghYTMIIW
# DwIBATCBlTB+MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4G
# A1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSgw
# JgYDVQQDEx9NaWNyb3NvZnQgQ29kZSBTaWduaW5nIFBDQSAyMDEwAhMzAAABtGsb
# Q7hXc1IAAAAAAAG0MA0GCWCGSAFlAwQCAQUAoIIBBDAZBgkqhkiG9w0BCQMxDAYK
# KwYBBAGCNwIBBDAcBgorBgEEAYI3AgELMQ4wDAYKKwYBBAGCNwIBFTAvBgkqhkiG
# 9w0BCQQxIgQg9dqdhF4OR7gDjF8vJdrLT9KZqbWkhiPfaXbvLkc5VLgwPAYKKwYB
# BAGCNwoDHDEuDCxWZEo2SmVNVXlyamx6UUtwY1hLbjNaQ0xOOGQzYmVIK2JjRUpU
# YTJlUlc0PTBaBgorBgEEAYI3AgEMMUwwSqAkgCIATQBpAGMAcgBvAHMAbwBmAHQA
# IABXAGkAbgBkAG8AdwBzoSKAIGh0dHA6Ly93d3cubWljcm9zb2Z0LmNvbS93aW5k
# b3dzMA0GCSqGSIb3DQEBAQUABIIBAJb+yVwOor9iy5buoDx008Ic3EwTuMpz+4oo
# rVlt3yywGfrI3nBqgqJJZ4g4LqXe9Ttt/zjRV72wVLTRQRPIlLSONc2stcwipBCc
# BU7hoSMGqWz0mcxa94cWTNAKg7GjghXcWIPB4y4g7OUSXtOmdkStrZPHnIemeHaI
# Ve5X9jhiXJj42XgA8OxHYYi0yNC1HNhwKOaRt7rQxlf2r8XqaG6QWR1GfjkfDKcg
# k7RjkpLSCta/LmNoT3fGA7QCa8T6h+563lqvDjwBEWeS6FlG6/n40ddU3eG2iY4Y
# YDBPGpSgB3IKpkuzVB8L+ibVtZBoU8hMxOL+1m8dftogj/4cemuhghNGMIITQgYK
# KwYBBAGCNwMDATGCEzIwghMuBgkqhkiG9w0BBwKgghMfMIITGwIBAzEPMA0GCWCG
# SAFlAwQCAQUAMIIBPAYLKoZIhvcNAQkQAQSgggErBIIBJzCCASMCAQEGCisGAQQB
# hFkKAwEwMTANBglghkgBZQMEAgEFAAQgDJJekUM0p7VltMIR7c4HLrWpeWqTinb0
# I1jKIiTHUOYCBlqyjf97PRgTMjAxODA0MTIwMTU5MjIuODU1WjAHAgEBgAID56CB
# uKSBtTCBsjELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNV
# BAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEMMAoG
# A1UECxMDQU9DMScwJQYDVQQLEx5uQ2lwaGVyIERTRSBFU046ODQzRC0zN0Y2LUYx
# MDQxJTAjBgNVBAMTHE1pY3Jvc29mdCBUaW1lLVN0YW1wIFNlcnZpY2Wggg7KMIIG
# cTCCBFmgAwIBAgIKYQmBKgAAAAAAAjANBgkqhkiG9w0BAQsFADCBiDELMAkGA1UE
# BhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAc
# BgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEyMDAGA1UEAxMpTWljcm9zb2Z0
# IFJvb3QgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IDIwMTAwHhcNMTAwNzAxMjEzNjU1
# WhcNMjUwNzAxMjE0NjU1WjB8MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGlu
# Z3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBv
# cmF0aW9uMSYwJAYDVQQDEx1NaWNyb3NvZnQgVGltZS1TdGFtcCBQQ0EgMjAxMDCC
# ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKkdDbx3EYo6IOz8E5f1+n9p
# lGt0VBDVpQoAgoX77XxoSyxfxcPlYcJ2tz5mK1vwFVMnBDEfQRsalR3OCROOfGEw
# WbEwRA/xYIiEVEMM1024OAizQt2TrNZzMFcmgqNFDdDq9UeBzb8kYDJYYEbyWEeG
# MoQedGFnkV+BVLHPk0ySwcSmXdFhE24oxhr5hoC732H8RsEnHSRnEnIaIYqvS2SJ
# UGKxXf13Hz3wV3WsvYpCTUBR0Q+cBj5nf/VmwAOWRH7v0Ev9buWayrGo8noqCjHw
# 2k4GkbaICDXoeByw6ZnNPOcvRLqn9NxkvaQBwSAJk3jN/LzAyURdXhacAQVPIk0C
# AwEAAaOCAeYwggHiMBAGCSsGAQQBgjcVAQQDAgEAMB0GA1UdDgQWBBTVYzpcijGQ
# 80N7fEYbxTNoWoVtVTAZBgkrBgEEAYI3FAIEDB4KAFMAdQBiAEMAQTALBgNVHQ8E
# BAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAfBgNVHSMEGDAWgBTV9lbLj+iiXGJo0T2U
# kFvXzpoYxDBWBgNVHR8ETzBNMEugSaBHhkVodHRwOi8vY3JsLm1pY3Jvc29mdC5j
# b20vcGtpL2NybC9wcm9kdWN0cy9NaWNSb29DZXJBdXRfMjAxMC0wNi0yMy5jcmww
# WgYIKwYBBQUHAQEETjBMMEoGCCsGAQUFBzAChj5odHRwOi8vd3d3Lm1pY3Jvc29m
# dC5jb20vcGtpL2NlcnRzL01pY1Jvb0NlckF1dF8yMDEwLTA2LTIzLmNydDCBoAYD
# VR0gAQH/BIGVMIGSMIGPBgkrBgEEAYI3LgMwgYEwPQYIKwYBBQUHAgEWMWh0dHA6
# Ly93d3cubWljcm9zb2Z0LmNvbS9QS0kvZG9jcy9DUFMvZGVmYXVsdC5odG0wQAYI
# KwYBBQUHAgIwNB4yIB0ATABlAGcAYQBsAF8AUABvAGwAaQBjAHkAXwBTAHQAYQB0
# AGUAbQBlAG4AdAAuIB0wDQYJKoZIhvcNAQELBQADggIBAAfmiFEN4sbgmD+BcQM9
# naOhIW+z66bM9TG+zwXiqf76V20ZMLPCxWbJat/15/B4vceoniXj+bzta1RXCCtR
# gkQS+7lTjMz0YBKKdsxAQEGb3FwX/1z5Xhc1mCRWS3TvQhDIr79/xn/yN31aPxzy
# mXlKkVIArzgPF/UveYFl2am1a+THzvbKegBvSzBEJCI8z+0DpZaPWSm8tv0E4XCf
# Mkon/VWvL/625Y4zu2JfmttXQOnxzplmkIz/amJ/3cVKC5Em4jnsGUpxY517IW3D
# nKOiPPp/fZZqkHimbdLhnPkd/DjYlPTGpQqWhqS9nhquBEKDuLWAmyI4ILUl5WTs
# 9/S/fmNZJQ96LjlXdqJxqgaKD4kWumGnEcua2A5HmoDF0M2n0O99g/DhO3EJ3110
# mCIIYdqwUB5vvfHhAN/nMQekkzr3ZUd46PioSKv33nJ+YWtvd6mBy6cJrDm77MbL
# 2IK0cs0d9LiFAR6A+xuJKlQ5slvayA1VmXqHczsI5pgt6o3gMy4SKfXAL1QnIffI
# rE7aKLixqduWsqdCosnPGUFN4Ib5KpqjEWYw07t0MkvfY3v1mYovG8chr1m1rtxE
# PJdQcdeh0sVV42neV8HR3jDA/czmTfsNv11P6Z0eGTgvvM9YBS7vDaBQNdrvCScc
# 1bN+NR4Iuto229Nfj950iEkSMIIE2TCCA8GgAwIBAgITMwAAAKlUcNl5wIRl4gAA
# AAAAqTANBgkqhkiG9w0BAQsFADB8MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2Fz
# aGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENv
# cnBvcmF0aW9uMSYwJAYDVQQDEx1NaWNyb3NvZnQgVGltZS1TdGFtcCBQQ0EgMjAx
# MDAeFw0xNjA5MDcxNzU2NTNaFw0xODA5MDcxNzU2NTNaMIGyMQswCQYDVQQGEwJV
# UzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UE
# ChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMQwwCgYDVQQLEwNBT0MxJzAlBgNVBAsT
# Hm5DaXBoZXIgRFNFIEVTTjo4NDNELTM3RjYtRjEwNDElMCMGA1UEAxMcTWljcm9z
# b2Z0IFRpbWUtU3RhbXAgU2VydmljZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC
# AQoCggEBAKyUhRim5VbTnd8DZy7bmJFHj5WJck2adPM0QN5rEafUx5J+Dv+fNTp/
# p2SMcZthxRlZPpRdkS9OwO3BQG4nB5HU5az2HjzOUBFTJ2iE0tJmpB9uHXspiybj
# Ca5050z8n4O3Rg3bFakN+1f3s5sQ3gwcpE0aw5YkRjYpkFzeHUfWlz9CjQf/qC8n
# jusSELFHQvcEvTk2xMW7wOy2vI6BnMndskA8PaPrG70wdf5bBXMdFDER1PA6v+zR
# E95EycT+SOFXxC54CBV+80UGNkm2tf1YHRHdiGQDQhA5765TFkYRW1x9Elp3SHZw
# KWEhGpNTnCJpbpy0eXqOTDiEFRlsX6ECAwEAAaOCARswggEXMB0GA1UdDgQWBBQP
# 1UH7u/Od9L5JUtqMN1fOZA/zfTAfBgNVHSMEGDAWgBTVYzpcijGQ80N7fEYbxTNo
# WoVtVTBWBgNVHR8ETzBNMEugSaBHhkVodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20v
# cGtpL2NybC9wcm9kdWN0cy9NaWNUaW1TdGFQQ0FfMjAxMC0wNy0wMS5jcmwwWgYI
# KwYBBQUHAQEETjBMMEoGCCsGAQUFBzAChj5odHRwOi8vd3d3Lm1pY3Jvc29mdC5j
# b20vcGtpL2NlcnRzL01pY1RpbVN0YVBDQV8yMDEwLTA3LTAxLmNydDAMBgNVHRMB
# Af8EAjAAMBMGA1UdJQQMMAoGCCsGAQUFBwMIMA0GCSqGSIb3DQEBCwUAA4IBAQAI
# mRilB4svciR1LI4ghAL9CqGvDuGwMhGGqsWGIJ276EQ7lcy1OueLRqUT04wgG6g6
# VxwV93NtjQuyB0VuhkV2JEa6dEzxNm9Snbbk5JSMySmriaq+coLTuptVq6CCOeAZ
# LtgRrUWJA0vAL+IAnrh7h9vK13W4LgS0jJM3ih4mROTZQWWO0UK5XhMQvBlndbvO
# hKnpH457zxuUuDldbVdZ7Ap+xtaVDTvn+ogvVqxVBFByZz+yAAPDjsL8PsiVq+DU
# Z7amNG20YAiyQonef2DaWCG804w4MlPyCszb3yS3AOuTSj8JejQ7S8Bza2pShFOh
# mwREWWFCrXLwQMBu/JqyoYIDdDCCAlwCAQEwgeKhgbikgbUwgbIxCzAJBgNVBAYT
# AlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYD
# VQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xDDAKBgNVBAsTA0FPQzEnMCUGA1UE
# CxMebkNpcGhlciBEU0UgRVNOOjg0M0QtMzdGNi1GMTA0MSUwIwYDVQQDExxNaWNy
# b3NvZnQgVGltZS1TdGFtcCBTZXJ2aWNloiUKAQEwCQYFKw4DAhoFAAMVAF06v1a9
# 4Q8pynIOZPd1O3gaIuv0oIHBMIG+pIG7MIG4MQswCQYDVQQGEwJVUzETMBEGA1UE
# CBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9z
# b2Z0IENvcnBvcmF0aW9uMQwwCgYDVQQLEwNBT0MxJzAlBgNVBAsTHm5DaXBoZXIg
# TlRTIEVTTjoyNjY1LTRDM0YtQzVERTErMCkGA1UEAxMiTWljcm9zb2Z0IFRpbWUg
# U291cmNlIE1hc3RlciBDbG9jazANBgkqhkiG9w0BAQUFAAIFAN55AIcwIhgPMjAx
# ODA0MTEyMTQ1NDNaGA8yMDE4MDQxMjIxNDU0M1owdDA6BgorBgEEAYRZCgQBMSww
# KjAKAgUA3nkAhwIBADAHAgEAAgIG2zAHAgEAAgIZtDAKAgUA3npSBwIBADA2Bgor
# BgEEAYRZCgQCMSgwJjAMBgorBgEEAYRZCgMBoAowCAIBAAIDHoCYoQowCAIBAAID
# HoSAMA0GCSqGSIb3DQEBBQUAA4IBAQBdYDFoYptSPo8UBkp6WL82pt0GqN00w9sx
# 0VHlQabRVZovFx9vQ7RyOyeBrCYpbjMU115YcjkmDNnr/cy86qz/l0R9NthSVEf0
# QfMPKw9GC6IFLPA/SH9BHh/6NHpkhd1lqD6vfxpD2Z99kQJx/jMtdggBiuD20jjH
# 2neyC5XeYycCSHaSlULU2BD//A6vXLYd5fm/75bdGjPqHFITukJo2Y9fzx4785bo
# NpHhvZSElrBkmFBu7frrdtRlaQAE90gR8c3RQbkU/1c2MldWlg7abTRD86PNq13Q
# iIrIC4NrD61CaSoXANGSg1BopkJtVAZv675uGTFApES2bGC/TcR9MYIC9TCCAvEC
# AQEwgZMwfDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNV
# BAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEmMCQG
# A1UEAxMdTWljcm9zb2Z0IFRpbWUtU3RhbXAgUENBIDIwMTACEzMAAACpVHDZecCE
# ZeIAAAAAAKkwDQYJYIZIAWUDBAIBBQCgggEyMBoGCSqGSIb3DQEJAzENBgsqhkiG
# 9w0BCRABBDAvBgkqhkiG9w0BCQQxIgQgoFsafMSgaWnMHPG90imL2AewJQb/8Pqs
# +rFqP3C5qpUwgeIGCyqGSIb3DQEJEAIMMYHSMIHPMIHMMIGxBBRdOr9WveEPKcpy
# DmT3dTt4GiLr9DCBmDCBgKR+MHwxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNo
# aW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29y
# cG9yYXRpb24xJjAkBgNVBAMTHU1pY3Jvc29mdCBUaW1lLVN0YW1wIFBDQSAyMDEw
# AhMzAAAAqVRw2XnAhGXiAAAAAACpMBYEFPIEAHEyZqdiP5MHkdpfNLWl3Q4DMA0G
# CSqGSIb3DQEBCwUABIIBADgE4clKOee1oKX3KZ54DRPVaoCgZ/NQMe41REmnbod1
# r2Ohoz3iay71a3eMFJz/NEO/M4PCOPMdcVJlq6SSMn/3YPCSf0aVGpsDwv9lhdA/
# cBes+Z67peE28SceLKRUQAgwogG8MrXZxBvn3M/lhRWARvODz9uaMt0AzuSb9d7G
# GWFyV6/4Jh4XdWsRsL8Gwen9ANhl0WCebQa+OBwA3G9oD/PkExXweT/np6WI9hM+
# Z9FKuaaK0d9ihRXS0JT/MsdeA/jFvjhpdJeuirQ/Bz2oh5XCv2YaWyYJpG5P5aXs
# DdkPWQjJZ8H03/eUpNFRtlsctoBPYpN0zoxVBU+GqEM=
# SIG # End signature block
