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
# MIIh6AYJKoZIhvcNAQcCoIIh2TCCIdUCAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCDbHDLYuqp9Du8W
# ZSYoLwjEyhkYywkzwemxEtpUqBqajKCCC4EwggUJMIID8aADAgECAhMzAAADg+rb
# u9lvIbj6AAAAAAODMA0GCSqGSIb3DQEBCwUAMH4xCzAJBgNVBAYTAlVTMRMwEQYD
# VQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNy
# b3NvZnQgQ29ycG9yYXRpb24xKDAmBgNVBAMTH01pY3Jvc29mdCBDb2RlIFNpZ25p
# bmcgUENBIDIwMTAwHhcNMjAwOTI0MTkxMDM1WhcNMjEwOTIzMTkxMDM1WjB/MQsw
# CQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
# ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSkwJwYDVQQDEyBNaWNy
# b3NvZnQgV2luZG93cyBLaXRzIFB1Ymxpc2hlcjCCASIwDQYJKoZIhvcNAQEBBQAD
# ggEPADCCAQoCggEBAKe3nYqa7gvoFrfJEv/ShtZ+iFysT9fLEWF0KRuIxsHnmREo
# O8C7fvPVxICiebjmyG5fMfkRYgHDTrDMo+3REcW33Knkk1A6CCUq21D4k06OEIDl
# GvqsrA8+M+y27/x2hQnm69JlHq2ph579/mSummMxyWaQ0LYOCvD3I6Zi3kFS8JQ0
# C2j8RpflSGh2Uh90X1o1lnmZjT52+Oo/33C2xGKtGaQ8srIByVkdXXTDSqEBwA0Y
# 8uM26umOIEhPmhBY5g4AZfwD3UGi/PB/VCHcexhI8Tph0imsUuh2V0lNrBLAlIFZ
# FZeYWO1CDTM0/xgY4oW3QJ3Bqmlfgv94gDgQshECAwEAAaOCAX0wggF5MB8GA1Ud
# JQQYMBYGCisGAQQBgjcKAxQGCCsGAQUFBwMDMB0GA1UdDgQWBBR4RevcOtDWvvVt
# GFRFemIpjDznDjBUBgNVHREETTBLpEkwRzEtMCsGA1UECxMkTWljcm9zb2Z0IEly
# ZWxhbmQgT3BlcmF0aW9ucyBMaW1pdGVkMRYwFAYDVQQFEw0yMjk5MDMrNDYxMjE0
# MB8GA1UdIwQYMBaAFOb8X3u7IgBY5HJOtfQhdCMy5u+sMFYGA1UdHwRPME0wS6BJ
# oEeGRWh0dHA6Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvY3JsL3Byb2R1Y3RzL01p
# Y0NvZFNpZ1BDQV8yMDEwLTA3LTA2LmNybDBaBggrBgEFBQcBAQROMEwwSgYIKwYB
# BQUHMAKGPmh0dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9wa2kvY2VydHMvTWljQ29k
# U2lnUENBXzIwMTAtMDctMDYuY3J0MAwGA1UdEwEB/wQCMAAwDQYJKoZIhvcNAQEL
# BQADggEBALsDxMmZj+9taKFEkr4WVO9H6Qdb9NulWVLK5PDz9X5h9sPsmapguzTE
# upIirmq8Oy3biVQ59UjZsEsGBYT+OwxlD1AGCYSJW7DZPCAZt3Jcc3SuCAkLzPNk
# x+oS3KGj5xqoUzMsgQt8ARVZ3eQgBZhU3OasPWukuzcfCciK/JH2bBnIFl9PSTiW
# stRSB8RGMh5BwAAjOgpLXVawgTWzpS4IpLDYWzdefJtye6BCs5YMaTgJhQAH1vUD
# pKEdQ3JvADzzzn1F0NjRqbRprK2AC2UYTIKf8J8yakCNou0JNhA3DuUe7rcceIwi
# 4n2lW9Rz/QTWtzOLehSH6kzt3zywN1kwggZwMIIEWKADAgECAgphDFJMAAAAAAAD
# MA0GCSqGSIb3DQEBCwUAMIGIMQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGlu
# Z3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBv
# cmF0aW9uMTIwMAYDVQQDEylNaWNyb3NvZnQgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRo
# b3JpdHkgMjAxMDAeFw0xMDA3MDYyMDQwMTdaFw0yNTA3MDYyMDUwMTdaMH4xCzAJ
# BgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25k
# MR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xKDAmBgNVBAMTH01pY3Jv
# c29mdCBDb2RlIFNpZ25pbmcgUENBIDIwMTAwggEiMA0GCSqGSIb3DQEBAQUAA4IB
# DwAwggEKAoIBAQDpDmRQeWe1xOP9CQBMnpSs91Zo6kTYz8VYT6mldnxtRbrTOZK0
# pB75+WWC5BfSj/1EnAjoZZPOLFWEv30I4y4rqEErGLeiS25JTGsVB97R0sKJHnGU
# zbV/S7SvCNjMiNZrF5Q6k84mP+zm/jSYV9UdXUn2siou1YW7WT/4kLQrg3TKK7M7
# RuPwRknBF2ZUyRy9HcRVYldy+Ge5JSA03l2mpZVeqyiAzdWynuUDtWPTshTIwciK
# JgpZfwfs/w7tgBI1TBKmvlJb9aba4IsLSHfWhUfVELnG6Krui2otBVxgxrQqW5wj
# HF9F4xoUHm83yxkzgGqJTaNqZmN4k9Uwz5UfAgMBAAGjggHjMIIB3zAQBgkrBgEE
# AYI3FQEEAwIBADAdBgNVHQ4EFgQU5vxfe7siAFjkck619CF0IzLm76wwGQYJKwYB
# BAGCNxQCBAweCgBTAHUAYgBDAEEwCwYDVR0PBAQDAgGGMA8GA1UdEwEB/wQFMAMB
# Af8wHwYDVR0jBBgwFoAU1fZWy4/oolxiaNE9lJBb186aGMQwVgYDVR0fBE8wTTBL
# oEmgR4ZFaHR0cDovL2NybC5taWNyb3NvZnQuY29tL3BraS9jcmwvcHJvZHVjdHMv
# TWljUm9vQ2VyQXV0XzIwMTAtMDYtMjMuY3JsMFoGCCsGAQUFBwEBBE4wTDBKBggr
# BgEFBQcwAoY+aHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraS9jZXJ0cy9NaWNS
# b29DZXJBdXRfMjAxMC0wNi0yMy5jcnQwgZ0GA1UdIASBlTCBkjCBjwYJKwYBBAGC
# Ny4DMIGBMD0GCCsGAQUFBwIBFjFodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vUEtJ
# L2RvY3MvQ1BTL2RlZmF1bHQuaHRtMEAGCCsGAQUFBwICMDQeMiAdAEwAZQBnAGEA
# bABfAFAAbwBsAGkAYwB5AF8AUwB0AGEAdABlAG0AZQBuAHQALiAdMA0GCSqGSIb3
# DQEBCwUAA4ICAQAadO9XTyl7xBaFeLhQ0yL8CZ2sgpf4NP8qLJeVEuXkv8+/k8jj
# NKnbgbjcHgC+0jVvr+V/eZV35QLU8evYzU4eG2GiwlojGvCMqGJRRWcI4z88HpP4
# MIUXyDlAptcOsyEp5aWhaYwik8x0mOehR0PyU6zADzBpf/7SJSBtb2HT3wfV2XIA
# LGmGdj1R26Y5SMk3YW0H3VMZy6fWYcK/4oOrD+Brm5XWfShRsIlKUaSabMi3H0oa
# Dmmp19zBftFJcKq2rbtyR2MX+qbWoqaG7KgQRJtjtrJpiQbHRoZ6GD/oxR0h1Xv5
# AiMtxUHLvx1MyBbvsZx//CJLSYpuFeOmf3Zb0VN5kYWd1dLbPXM18zyuVLJSR2rA
# qhOV0o4R2plnXjKM+zeF0dx1hZyHxlpXhcK/3Q2PjJst67TuzyfTtV5p+qQWBAGn
# JGdzz01Ptt4FVpd69+lSTfR3BU+FxtgL8Y7tQgnRDXbjI1Z4IiY2vsqxjG6qHeSF
# 2kczYo+kyZEzX3EeQK+YZcki6EIhJYocLWDZN4lBiSoWD9dhPJRoYFLv1keZoIBA
# 7hWBdz6c4FMYGlAdOJWbHmYzEyc5F3iHNs5Ow1+y9T1HU7bg5dsLYT0q15Iszjda
# PkBCMaQfEAjCVpy/JF1RAp1qedIX09rBlI4HeyVxRKsGaubUxt8jmpZ1xTGCFb0w
# ghW5AgEBMIGVMH4xCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAw
# DgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24x
# KDAmBgNVBAMTH01pY3Jvc29mdCBDb2RlIFNpZ25pbmcgUENBIDIwMTACEzMAAAOD
# 6tu72W8huPoAAAAAA4MwDQYJYIZIAWUDBAIBBQCgggEEMBkGCSqGSIb3DQEJAzEM
# BgorBgEEAYI3AgEEMBwGCisGAQQBgjcCAQsxDjAMBgorBgEEAYI3AgEVMC8GCSqG
# SIb3DQEJBDEiBCD12p2EXg5HuAOMXy8l2stP0pmptaSGI99pdu8uRzlUuDA8Bgor
# BgEEAYI3CgMcMS4MLFZkSjZKZU1VeXJqbHpRS3BjWEtuM1pDTE44ZDNiZUgrYmNF
# SlRhMmVSVzQ9MFoGCisGAQQBgjcCAQwxTDBKoCSAIgBNAGkAYwByAG8AcwBvAGYA
# dAAgAFcAaQBuAGQAbwB3AHOhIoAgaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3dp
# bmRvd3MwDQYJKoZIhvcNAQEBBQAEggEAU8PW0grTtgZFxpgRBiGco4ldV+6d8Tm5
# ejC548kUr+4G/uNwwuvuJX8bLeiHV/XaOj84M4B5u2a8TdIyNubTWz/Hk8fszl18
# 4F7tj0fLHGeynxo17es1jDycv+UHRk6PwKEJFGCuFZ54rUnZhTLcTrg8tXyCq+2b
# kcGlNX52dgjBR2T41XMSf2lSGJKYR5pIGmzRDspaUXLz7aQ6aW/KSWWfR/HUzB/p
# cTzHbftCcwMQgjp8HH3MX+uADfCzK/DRgCrE6My//PxL4Hr615V9cjvDJjBEKsru
# RkBMczUsB9GKXM80fTaiLXBVjs5MoYG1QkJ4ZO+Y+GLN28D28VrLaqGCEvAwghLs
# BgorBgEEAYI3AwMBMYIS3DCCEtgGCSqGSIb3DQEHAqCCEskwghLFAgEDMQ8wDQYJ
# YIZIAWUDBAIBBQAwggFUBgsqhkiG9w0BCRABBKCCAUMEggE/MIIBOwIBAQYKKwYB
# BAGEWQoDATAxMA0GCWCGSAFlAwQCAQUABCBLmxy0lc6J4J7zInh6CQAYASl2JEKv
# YvxbEVAGZH6hxQIGX7vOv9rSGBIyMDIwMTIwMjEyNDA0OS4wNFowBIACAfSggdSk
# gdEwgc4xCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQH
# EwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xKTAnBgNV
# BAsTIE1pY3Jvc29mdCBPcGVyYXRpb25zIFB1ZXJ0byBSaWNvMSYwJAYDVQQLEx1U
# aGFsZXMgVFNTIEVTTjpGN0E2LUUyNTEtMTUwQTElMCMGA1UEAxMcTWljcm9zb2Z0
# IFRpbWUtU3RhbXAgU2VydmljZaCCDkQwggT1MIID3aADAgECAhMzAAABJYvei2xy
# JjHdAAAAAAElMA0GCSqGSIb3DQEBCwUAMHwxCzAJBgNVBAYTAlVTMRMwEQYDVQQI
# EwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3Nv
# ZnQgQ29ycG9yYXRpb24xJjAkBgNVBAMTHU1pY3Jvc29mdCBUaW1lLVN0YW1wIFBD
# QSAyMDEwMB4XDTE5MTIxOTAxMTQ1OFoXDTIxMDMxNzAxMTQ1OFowgc4xCzAJBgNV
# BAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4w
# HAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xKTAnBgNVBAsTIE1pY3Jvc29m
# dCBPcGVyYXRpb25zIFB1ZXJ0byBSaWNvMSYwJAYDVQQLEx1UaGFsZXMgVFNTIEVT
# TjpGN0E2LUUyNTEtMTUwQTElMCMGA1UEAxMcTWljcm9zb2Z0IFRpbWUtU3RhbXAg
# U2VydmljZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANB7H2N2YFvs
# 4cnBJiYxSitk3ABy/xXLfpOUm7NxXHsb6UWq3bONY4yVI4ySbVegC4nxVnlKEF50
# ANcMYMrEc1mEu7cRbzHmi38g6TqLMtOUAW28hc6DNez8do4zvZccrKQxkcB0v9+l
# m0BIzk9qWaxdfg6XyVeSb2NHnkrnoLur36ENT7a2MYdoTVlaVpuU1RcGFpmC0IkJ
# 3rRTJm+Ajv+43Nxp+PT9XDZtqK32cMBV3bjK39cJmcdjfJftmweMi4emyX4+kNdq
# LUPB72nSvIJmyX1I4wd7G0gd72qVNu1Zgnxa1Yugf10QxDFUueY88M5WYGPstmFK
# OLfw31WnP8UCAwEAAaOCARswggEXMB0GA1UdDgQWBBTzqsrlByb5ATk0FcYI8iII
# F0Mk+DAfBgNVHSMEGDAWgBTVYzpcijGQ80N7fEYbxTNoWoVtVTBWBgNVHR8ETzBN
# MEugSaBHhkVodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20vcGtpL2NybC9wcm9kdWN0
# cy9NaWNUaW1TdGFQQ0FfMjAxMC0wNy0wMS5jcmwwWgYIKwYBBQUHAQEETjBMMEoG
# CCsGAQUFBzAChj5odHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL01p
# Y1RpbVN0YVBDQV8yMDEwLTA3LTAxLmNydDAMBgNVHRMBAf8EAjAAMBMGA1UdJQQM
# MAoGCCsGAQUFBwMIMA0GCSqGSIb3DQEBCwUAA4IBAQCTHFk8YSAiACGypk1NmTnx
# XW9CInmNrbEeXlOoYDofCPlKKguDcVIuJOYZX4G0WWlhS2Sd4HiOtmy42ky19tMx
# 0bun/EDIhW3C9edNeoqUIPVP0tyv3ilV53McYnMvVNg1DJkkGi4J/OSCTNxw64U5
# 95Y9+cxOIjlQFbk52ajIc9BYNIYehuhbV1Mqpd4m25UNNhsdMqzjON8IEwWObKVG
# 7nZmmLP70wF5GPiIB6i7QX/fG8jN6mggqBRYJn2aZWJYSRXAK1MZtXx4rvcp4QTS
# 18xT9hjZSagY9zxjBu6sMR96V6Atb5geR+twYAaV+0Kaq0504t6CEugbRRvH8Hux
# MIIGcTCCBFmgAwIBAgIKYQmBKgAAAAAAAjANBgkqhkiG9w0BAQsFADCBiDELMAkG
# A1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQx
# HjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEyMDAGA1UEAxMpTWljcm9z
# b2Z0IFJvb3QgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IDIwMTAwHhcNMTAwNzAxMjEz
# NjU1WhcNMjUwNzAxMjE0NjU1WjB8MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2Fz
# aGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENv
# cnBvcmF0aW9uMSYwJAYDVQQDEx1NaWNyb3NvZnQgVGltZS1TdGFtcCBQQ0EgMjAx
# MDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKkdDbx3EYo6IOz8E5f1
# +n9plGt0VBDVpQoAgoX77XxoSyxfxcPlYcJ2tz5mK1vwFVMnBDEfQRsalR3OCROO
# fGEwWbEwRA/xYIiEVEMM1024OAizQt2TrNZzMFcmgqNFDdDq9UeBzb8kYDJYYEby
# WEeGMoQedGFnkV+BVLHPk0ySwcSmXdFhE24oxhr5hoC732H8RsEnHSRnEnIaIYqv
# S2SJUGKxXf13Hz3wV3WsvYpCTUBR0Q+cBj5nf/VmwAOWRH7v0Ev9buWayrGo8noq
# CjHw2k4GkbaICDXoeByw6ZnNPOcvRLqn9NxkvaQBwSAJk3jN/LzAyURdXhacAQVP
# Ik0CAwEAAaOCAeYwggHiMBAGCSsGAQQBgjcVAQQDAgEAMB0GA1UdDgQWBBTVYzpc
# ijGQ80N7fEYbxTNoWoVtVTAZBgkrBgEEAYI3FAIEDB4KAFMAdQBiAEMAQTALBgNV
# HQ8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAfBgNVHSMEGDAWgBTV9lbLj+iiXGJo
# 0T2UkFvXzpoYxDBWBgNVHR8ETzBNMEugSaBHhkVodHRwOi8vY3JsLm1pY3Jvc29m
# dC5jb20vcGtpL2NybC9wcm9kdWN0cy9NaWNSb29DZXJBdXRfMjAxMC0wNi0yMy5j
# cmwwWgYIKwYBBQUHAQEETjBMMEoGCCsGAQUFBzAChj5odHRwOi8vd3d3Lm1pY3Jv
# c29mdC5jb20vcGtpL2NlcnRzL01pY1Jvb0NlckF1dF8yMDEwLTA2LTIzLmNydDCB
# oAYDVR0gAQH/BIGVMIGSMIGPBgkrBgEEAYI3LgMwgYEwPQYIKwYBBQUHAgEWMWh0
# dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9QS0kvZG9jcy9DUFMvZGVmYXVsdC5odG0w
# QAYIKwYBBQUHAgIwNB4yIB0ATABlAGcAYQBsAF8AUABvAGwAaQBjAHkAXwBTAHQA
# YQB0AGUAbQBlAG4AdAAuIB0wDQYJKoZIhvcNAQELBQADggIBAAfmiFEN4sbgmD+B
# cQM9naOhIW+z66bM9TG+zwXiqf76V20ZMLPCxWbJat/15/B4vceoniXj+bzta1RX
# CCtRgkQS+7lTjMz0YBKKdsxAQEGb3FwX/1z5Xhc1mCRWS3TvQhDIr79/xn/yN31a
# PxzymXlKkVIArzgPF/UveYFl2am1a+THzvbKegBvSzBEJCI8z+0DpZaPWSm8tv0E
# 4XCfMkon/VWvL/625Y4zu2JfmttXQOnxzplmkIz/amJ/3cVKC5Em4jnsGUpxY517
# IW3DnKOiPPp/fZZqkHimbdLhnPkd/DjYlPTGpQqWhqS9nhquBEKDuLWAmyI4ILUl
# 5WTs9/S/fmNZJQ96LjlXdqJxqgaKD4kWumGnEcua2A5HmoDF0M2n0O99g/DhO3EJ
# 3110mCIIYdqwUB5vvfHhAN/nMQekkzr3ZUd46PioSKv33nJ+YWtvd6mBy6cJrDm7
# 7MbL2IK0cs0d9LiFAR6A+xuJKlQ5slvayA1VmXqHczsI5pgt6o3gMy4SKfXAL1Qn
# IffIrE7aKLixqduWsqdCosnPGUFN4Ib5KpqjEWYw07t0MkvfY3v1mYovG8chr1m1
# rtxEPJdQcdeh0sVV42neV8HR3jDA/czmTfsNv11P6Z0eGTgvvM9YBS7vDaBQNdrv
# CScc1bN+NR4Iuto229Nfj950iEkSoYIC0jCCAjsCAQEwgfyhgdSkgdEwgc4xCzAJ
# BgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25k
# MR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xKTAnBgNVBAsTIE1pY3Jv
# c29mdCBPcGVyYXRpb25zIFB1ZXJ0byBSaWNvMSYwJAYDVQQLEx1UaGFsZXMgVFNT
# IEVTTjpGN0E2LUUyNTEtMTUwQTElMCMGA1UEAxMcTWljcm9zb2Z0IFRpbWUtU3Rh
# bXAgU2VydmljZaIjCgEBMAcGBSsOAwIaAxUARdMv4VBtzYb7cxde8hEpWvahcKeg
# gYMwgYCkfjB8MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4G
# A1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSYw
# JAYDVQQDEx1NaWNyb3NvZnQgVGltZS1TdGFtcCBQQ0EgMjAxMDANBgkqhkiG9w0B
# AQUFAAIFAONxgXQwIhgPMjAyMDEyMDIwNjU5MDBaGA8yMDIwMTIwMzA2NTkwMFow
# dzA9BgorBgEEAYRZCgQBMS8wLTAKAgUA43GBdAIBADAKAgEAAgIUnAIB/zAHAgEA
# AgITjjAKAgUA43LS9AIBADA2BgorBgEEAYRZCgQCMSgwJjAMBgorBgEEAYRZCgMC
# oAowCAIBAAIDB6EgoQowCAIBAAIDAYagMA0GCSqGSIb3DQEBBQUAA4GBADlmP56h
# Tew3n1SoxqphFOskVQvHPyXogFK/dCldkP3NrhO+LlyMZMuMKpKgA19n+SRg6Sya
# L0PFtndLGcvLVnVwA5ihRiYR/vGq8FMXM9kpce1yBA+495SggT5+4VG9FBr6ox9X
# lfYuyyydeeO6FlfG7Xh0Cu2cr9URnXtipfbpMYIDDTCCAwkCAQEwgZMwfDELMAkG
# A1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQx
# HjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEmMCQGA1UEAxMdTWljcm9z
# b2Z0IFRpbWUtU3RhbXAgUENBIDIwMTACEzMAAAEli96LbHImMd0AAAAAASUwDQYJ
# YIZIAWUDBAIBBQCgggFKMBoGCSqGSIb3DQEJAzENBgsqhkiG9w0BCRABBDAvBgkq
# hkiG9w0BCQQxIgQgb0IB5Co1lz8L9hzww3G/3GkomTrPiBkWzMTidS/G2sAwgfoG
# CyqGSIb3DQEJEAIvMYHqMIHnMIHkMIG9BCBd38ayLm8wX/qJfYIOH5V+YvlG+poW
# QXCW6LKN70H3DjCBmDCBgKR+MHwxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNo
# aW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29y
# cG9yYXRpb24xJjAkBgNVBAMTHU1pY3Jvc29mdCBUaW1lLVN0YW1wIFBDQSAyMDEw
# AhMzAAABJYvei2xyJjHdAAAAAAElMCIEIB3ZVyDXLMuLlQSN4cB9Dv0MSCQgYsBf
# HygUlvpjiR/4MA0GCSqGSIb3DQEBCwUABIIBAHpFWgebQZAbvGHKVQFJ9dyDAMNS
# Sr3DIr/i0RHXRcxfXkjfX4IOSCOMSH7GGQTtZc3tZGTqIhwZ459wK6cNbEFm0Jnf
# Sc8fhy4BXZlePPwD4mxqg0YRlpTyXHFTtRNFoNr43DJxCOtp2MfDLOGch9BmZrBs
# AmmhETiB11yhh02SZzXzOVBR+a2BSP/P7YNqmYCk5XW0/VYpsQHke2zwQM+pITDe
# L0IErZvv36iozefdYGE+C3P+VfHQNS3uaAs0VRTxDjQcFTwST0ioWUxKVyLwOi8S
# u8xV9KhQjW8Xq0xbCW/oZiGF5F/MHRimx8bsXEK93UOf9DvmEf5N3Inm9No=
# SIG # End signature block
