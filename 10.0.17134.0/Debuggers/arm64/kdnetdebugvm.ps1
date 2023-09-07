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
# BAGCNwoDHDEuDCxDakRUdGxwRjR4MVNrU1E2ZERQRFdkblkydHo2bFBCTXlDeFpW
# MkdqQVRNPTBaBgorBgEEAYI3AgEMMUwwSqAkgCIATQBpAGMAcgBvAHMAbwBmAHQA
# IABXAGkAbgBkAG8AdwBzoSKAIGh0dHA6Ly93d3cubWljcm9zb2Z0LmNvbS93aW5k
# b3dzMA0GCSqGSIb3DQEBAQUABIIBAESLV6HeW7NT3L/Icc+N+upQ6X+9uA3/L/Bl
# vAn/neR6nk9x6DPIu3SxDzy/xuupbwyVAG7FlBMHr0UyQahbo4sGLBpFx5th+xCP
# xAb/8eOfK8UiSszzll7iN+iDtZE/ZkhoI8iYqDclcXkUc0Ye6JhazvD32AImUmhj
# atVYL3Jf93Z3/DrwirqcUVSObtulM7KMBYXhhzlOvbFpkQkbZy7h3ZOZ6ucDGg+d
# kKAb00VVS1FNRbn2w3VU2hzYjG+7qMyPfJ/Jbev+e5uz3VLp9JGOaVB7j4nFXdck
# ooadWzONIhO+oGzxkCsiiZajbnK+8ZrvntxFUGVZPkbRsy5rYkqhghNGMIITQgYK
# KwYBBAGCNwMDATGCEzIwghMuBgkqhkiG9w0BBwKgghMfMIITGwIBAzEPMA0GCWCG
# SAFlAwQCAQUAMIIBPAYLKoZIhvcNAQkQAQSgggErBIIBJzCCASMCAQEGCisGAQQB
# hFkKAwEwMTANBglghkgBZQMEAgEFAAQg9O+Vs/LOwkY0tEscLCmZIfbWjoRZR6dE
# cbmeiAUPeKoCBlqygTOdQhgTMjAxODA0MTIwNjM2MzEuOTI0WjAHAgEBgAIB9KCB
# uKSBtTCBsjELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNV
# BAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEMMAoG
# A1UECxMDQU9DMScwJQYDVQQLEx5uQ2lwaGVyIERTRSBFU046MTJCNC0yRDVGLTg3
# RDQxJTAjBgNVBAMTHE1pY3Jvc29mdCBUaW1lLVN0YW1wIFNlcnZpY2Wggg7KMIIG
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
# 1bN+NR4Iuto229Nfj950iEkSMIIE2TCCA8GgAwIBAgITMwAAAKdkXcUoDwE9RAAA
# AAAApzANBgkqhkiG9w0BAQsFADB8MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2Fz
# aGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENv
# cnBvcmF0aW9uMSYwJAYDVQQDEx1NaWNyb3NvZnQgVGltZS1TdGFtcCBQQ0EgMjAx
# MDAeFw0xNjA5MDcxNzU2NTJaFw0xODA5MDcxNzU2NTJaMIGyMQswCQYDVQQGEwJV
# UzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UE
# ChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMQwwCgYDVQQLEwNBT0MxJzAlBgNVBAsT
# Hm5DaXBoZXIgRFNFIEVTTjoxMkI0LTJENUYtODdENDElMCMGA1UEAxMcTWljcm9z
# b2Z0IFRpbWUtU3RhbXAgU2VydmljZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC
# AQoCggEBAKbqGlDAX0/SU/mh7AR8DdcgVzEhUdjH7gMKLnjOiy4iI2uWNhAfClF6
# heddFP5zstfSfGNVwQq4xpQ06LiJ2RecCbEYoZE/Q1ZVVtlbxU/Wg37k6BCIG0x6
# G5Ci1LUC09H4v5vO/zVq1S6OTJNGerN9TlLQGBHYzJfMTJTkcotQ/OrSUgkQOIMR
# uJYDMThcQY06KYcRWBOjDq19rd765m7K0Lihc22+tamlZ0lrB3x5iLRmNvIxJzgU
# 4jUfwSddeMjiKjJrZ4FGUwXWe0M1akQw7bsh7TdJf7JECZvaWcArPpL4DsAmSxeK
# 7wjTVOeMC4+KjVL2oHmoT2YGMrmLKvECAwEAAaOCARswggEXMB0GA1UdDgQWBBR8
# DUoLfftP/14QLD15byBNQlCIXTAfBgNVHSMEGDAWgBTVYzpcijGQ80N7fEYbxTNo
# WoVtVTBWBgNVHR8ETzBNMEugSaBHhkVodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20v
# cGtpL2NybC9wcm9kdWN0cy9NaWNUaW1TdGFQQ0FfMjAxMC0wNy0wMS5jcmwwWgYI
# KwYBBQUHAQEETjBMMEoGCCsGAQUFBzAChj5odHRwOi8vd3d3Lm1pY3Jvc29mdC5j
# b20vcGtpL2NlcnRzL01pY1RpbVN0YVBDQV8yMDEwLTA3LTAxLmNydDAMBgNVHRMB
# Af8EAjAAMBMGA1UdJQQMMAoGCCsGAQUFBwMIMA0GCSqGSIb3DQEBCwUAA4IBAQCa
# xiRL55eyUgFkhw/iQpMoVnl29PkKFWdNeg6JgDw/M0LejnLyXKbzeLO5sIXSjyXt
# 24q94Sx9VmAEEs8n82N4+EZUVEoW2mvqXLv7hnpsXn9Nc2KgNp2w3rPpzxJijPrw
# nGwkDGuILpGXvt9OMoxcbieEJ3mbncvf6OwRCwAJXHovaiX20vY6KodtR3bMVa9O
# tvHVehqz+ZwdCEgo/XH/XgebJUKwo/gL/WHSX7p06GHV8+LgRL8pDeOK8G1djFri
# 5Q4NgQfRK47SeVCZrQGiyzUBur4AQoMjcbYdYIeqjFv+e+1whg4IcbVXhS+UECNt
# tB+8o0a35RZLrvCutrpOoYIDdDCCAlwCAQEwgeKhgbikgbUwgbIxCzAJBgNVBAYT
# AlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYD
# VQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xDDAKBgNVBAsTA0FPQzEnMCUGA1UE
# CxMebkNpcGhlciBEU0UgRVNOOjEyQjQtMkQ1Ri04N0Q0MSUwIwYDVQQDExxNaWNy
# b3NvZnQgVGltZS1TdGFtcCBTZXJ2aWNloiUKAQEwCQYFKw4DAhoFAAMVAOSCLp0y
# Nm10ADIPHsEhtA3rDHfZoIHBMIG+pIG7MIG4MQswCQYDVQQGEwJVUzETMBEGA1UE
# CBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9z
# b2Z0IENvcnBvcmF0aW9uMQwwCgYDVQQLEwNBT0MxJzAlBgNVBAsTHm5DaXBoZXIg
# TlRTIEVTTjoyNjY1LTRDM0YtQzVERTErMCkGA1UEAxMiTWljcm9zb2Z0IFRpbWUg
# U291cmNlIE1hc3RlciBDbG9jazANBgkqhkiG9w0BAQUFAAIFAN55AHUwIhgPMjAx
# ODA0MTEyMTQ1MjVaGA8yMDE4MDQxMjIxNDUyNVowdDA6BgorBgEEAYRZCgQBMSww
# KjAKAgUA3nkAdQIBADAHAgEAAgIFTDAHAgEAAgIZgzAKAgUA3npR9QIBADA2Bgor
# BgEEAYRZCgQCMSgwJjAMBgorBgEEAYRZCgMBoAowCAIBAAIDFuNgoQowCAIBAAID
# B6EgMA0GCSqGSIb3DQEBBQUAA4IBAQAR2s+q+pncRxRMfsK7l1vuYHbbgY0PvGgc
# a1croTXvMnijwy717cMcy/l9MZhJjFHbV6iz65scRq1Btg/q8ekhONiHj73Q1je6
# Vy+SK2ZlZ0gwdr1EIT5qNy8+6sqnxQnPOOcS3IiBbwH1K53n96MRBIyWICAqHU/L
# JaektDm525dP2uEK4Hqte6sje8YQc0gRse716OZ3dykjy96Uw3WSadsVs7y6ATJb
# CHdl12T0nUAfK8+JMk2EqAbRwk3B/5BRW9kCqTgAJg1E7y6O2NlmuMIAlVu3SBO5
# dy7K5EVh4W5Nf0dMQOz26CFQwOsOb9nERDHIaTSQ8Bsei4JAbIgLMYIC9TCCAvEC
# AQEwgZMwfDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNV
# BAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEmMCQG
# A1UEAxMdTWljcm9zb2Z0IFRpbWUtU3RhbXAgUENBIDIwMTACEzMAAACnZF3FKA8B
# PUQAAAAAAKcwDQYJYIZIAWUDBAIBBQCgggEyMBoGCSqGSIb3DQEJAzENBgsqhkiG
# 9w0BCRABBDAvBgkqhkiG9w0BCQQxIgQgnu/BuAKMjdxPXV+KMFyePP4nSgMWyvEi
# EQ3X5BcNQvgwgeIGCyqGSIb3DQEJEAIMMYHSMIHPMIHMMIGxBBTkgi6dMjZtdAAy
# Dx7BIbQN6wx32TCBmDCBgKR+MHwxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNo
# aW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29y
# cG9yYXRpb24xJjAkBgNVBAMTHU1pY3Jvc29mdCBUaW1lLVN0YW1wIFBDQSAyMDEw
# AhMzAAAAp2RdxSgPAT1EAAAAAACnMBYEFMj4qhhuPJ1exhVCqVeVw1U1zyIRMA0G
# CSqGSIb3DQEBCwUABIIBAGFbIHPVzKAnIZY6lqpvZRaf1h7nwT04dd1izBFC4b5h
# jYgAJ0c3Vza5dG0n1cksmZcB6T6Q1Hl98FxOv4z8NngHwqkZ06H7/jC757BIlCy+
# RzXD07zrXpy9gVLujTte+Ln9yEdqV7l+Oh2LQLsKyRU2+NsWyV+5YNQg2Ljyys4Y
# 3Q8B7MTqT7bvBpUfrasawipVcrZgKzgkTj047WiLKk417qEVjINQqvVh2QVsvhr5
# i7wAf0aSrNiR8b3SxwMDOgCe6VR5EguZ/kgez7n5I4TBRI9QldCNaZB69CfI5rph
# 9QWVPpywOH4pAsxMwHp6IbihNjyme0M67ygGmKQafUM=
# SIG # End signature block
