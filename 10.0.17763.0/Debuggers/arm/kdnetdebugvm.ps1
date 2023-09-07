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
# MIIirAYJKoZIhvcNAQcCoIIinTCCIpkCAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCDbHDLYuqp9Du8W
# ZSYoLwjEyhkYywkzwemxEtpUqBqajKCCC4EwggUJMIID8aADAgECAhMzAAACJG2S
# 5VjKdf54AAAAAAIkMA0GCSqGSIb3DQEBCwUAMH4xCzAJBgNVBAYTAlVTMRMwEQYD
# VQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNy
# b3NvZnQgQ29ycG9yYXRpb24xKDAmBgNVBAMTH01pY3Jvc29mdCBDb2RlIFNpZ25p
# bmcgUENBIDIwMTAwHhcNMTgwNTMxMTczNzAyWhcNMTkwNTI5MTczNzAyWjB/MQsw
# CQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
# ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSkwJwYDVQQDEyBNaWNy
# b3NvZnQgV2luZG93cyBLaXRzIFB1Ymxpc2hlcjCCASIwDQYJKoZIhvcNAQEBBQAD
# ggEPADCCAQoCggEBALUAO1XlZu1u14a2BT1w1Rf5vQ4YH9YkJAx2KWLJIH+IAKcj
# pAFqdYJe3YYqr8fV1TjB5GR0UkNA13z2/iGmnHEUV5mmaFV9BqlEAl/uCKr2R7cc
# 6OPwnu+Ou5pJ1QRFZ2uk+ZMjgPZEPxpIitV38reCwgxQRbyZCNR/jiorsfsH1kmz
# j3hRrRzwWzuAxuwZb7r7AOkxgB156LYTiTYY7CFMRnAScVrAps2DqY3JiI/kzloU
# v5gQKwp1oXfXfp96vqWdpKNlWa2+VfLxj4BF6+kC1o0DkZYFl4ME/2F38Xuw96XF
# GCEmXGiF5pwjHrQDgg/FHbIABV+ZpSgdPD0pLtkCAwEAAaOCAX0wggF5MB8GA1Ud
# JQQYMBYGCisGAQQBgjcKAxQGCCsGAQUFBwMDMB0GA1UdDgQWBBT03vBzGFpXavw+
# EO3eYmj9DrbSmjBUBgNVHREETTBLpEkwRzEtMCsGA1UECxMkTWljcm9zb2Z0IEly
# ZWxhbmQgT3BlcmF0aW9ucyBMaW1pdGVkMRYwFAYDVQQFEw0yMjk5MDMrNDM2MDg5
# MB8GA1UdIwQYMBaAFOb8X3u7IgBY5HJOtfQhdCMy5u+sMFYGA1UdHwRPME0wS6BJ
# oEeGRWh0dHA6Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvY3JsL3Byb2R1Y3RzL01p
# Y0NvZFNpZ1BDQV8yMDEwLTA3LTA2LmNybDBaBggrBgEFBQcBAQROMEwwSgYIKwYB
# BQUHMAKGPmh0dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9wa2kvY2VydHMvTWljQ29k
# U2lnUENBXzIwMTAtMDctMDYuY3J0MAwGA1UdEwEB/wQCMAAwDQYJKoZIhvcNAQEL
# BQADggEBAORp1AJcig5+KRMkkh5exzIFd+O7ccdVf5fgpmzZVrLAU2cMIgkbjX2p
# 6V8wbDM5LY2/VqNq6Twl/PdKDf8EYAIxbZ+J32AFzNH/sgBcke0qDGQ0HT+3RgfX
# R6n/qWQrScz/w70dahX9zuLgt0h9OJ4XswMBSukyTBVfQARaTfy3Pj3tBU+QPBHt
# SDDYA5LmqdGLB68K8CTrua0pg8p3Ux1W7Tp7d0X+KCU1m68FYh4oVrPR27SwGFeu
# ak7+uLH8LV7VOmD52m/y3XfW7+sjNoVBix1s1pJns19tRei1HbCdaWAGvw7y5Pex
# 2m96SuVNnYkDS6Y9lfChl6GHiJxn3Q0wggZwMIIEWKADAgECAgphDFJMAAAAAAAD
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
# PkBCMaQfEAjCVpy/JF1RAp1qedIX09rBlI4HeyVxRKsGaubUxt8jmpZ1xTGCFoEw
# ghZ9AgEBMIGVMH4xCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAw
# DgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24x
# KDAmBgNVBAMTH01pY3Jvc29mdCBDb2RlIFNpZ25pbmcgUENBIDIwMTACEzMAAAIk
# bZLlWMp1/ngAAAAAAiQwDQYJYIZIAWUDBAIBBQCgggEEMBkGCSqGSIb3DQEJAzEM
# BgorBgEEAYI3AgEEMBwGCisGAQQBgjcCAQsxDjAMBgorBgEEAYI3AgEVMC8GCSqG
# SIb3DQEJBDEiBCD12p2EXg5HuAOMXy8l2stP0pmptaSGI99pdu8uRzlUuDA8Bgor
# BgEEAYI3CgMcMS4MLFZkSjZKZU1VeXJqbHpRS3BjWEtuM1pDTE44ZDNiZUgrYmNF
# SlRhMmVSVzQ9MFoGCisGAQQBgjcCAQwxTDBKoCSAIgBNAGkAYwByAG8AcwBvAGYA
# dAAgAFcAaQBuAGQAbwB3AHOhIoAgaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3dp
# bmRvd3MwDQYJKoZIhvcNAQEBBQAEggEAY72pVnWtY7bbskpmesKAxx+rNi34b8Hj
# QnRNISVzuEpflay8BnUS8qAKeH5m6ojBOhFwD+lSaHBr8d7Gl+b8hK0g+nUfiizs
# Nuy+kCSQ1CALmkodRDk0Kq+MQeHxC1jUC4vG9GFsR2uek/9n12cl5kCHaMAEhont
# DOTJ+sdMlZCf4245r74VpN+AYj+Tpn3vpSA11CMKf43nJJM8guI6Bvw31hLrS37o
# n4hBXj5jmVnKUDtqrh3UwgU5sTBp5Y9oMyvhCMUbf/2N8cBj4VnJg6CTnnN+1eZG
# ssI0Jzna+S41L+oWiQoAvDoM3duz016qeN+QQiOlXUch010GOGVwqKGCE7QwghOw
# BgorBgEEAYI3AwMBMYIToDCCE5wGCSqGSIb3DQEHAqCCE40wghOJAgEDMQ8wDQYJ
# YIZIAWUDBAIBBQAwggFVBgsqhkiG9w0BCRABBKCCAUQEggFAMIIBPAIBAQYKKwYB
# BAGEWQoDATAxMA0GCWCGSAFlAwQCAQUABCDRJgCc+P6pB/qdtNka+wz57D2a4Iqo
# kcu6Kv6REj515AIGW4hFfo77GBMyMDE4MDkxNTA4MzUwNi41NTZaMASAAgH0oIHU
# pIHRMIHOMQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UE
# BxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSkwJwYD
# VQQLEyBNaWNyb3NvZnQgT3BlcmF0aW9ucyBQdWVydG8gUmljbzEmMCQGA1UECxMd
# VGhhbGVzIFRTUyBFU046MTQ4Qy1DNEI5LTIwNjYxJTAjBgNVBAMTHE1pY3Jvc29m
# dCBUaW1lLVN0YW1wIFNlcnZpY2Wggg8fMIIE9TCCA92gAwIBAgITMwAAANWnI+V4
# lWoJ/wAAAAAA1TANBgkqhkiG9w0BAQsFADB8MQswCQYDVQQGEwJVUzETMBEGA1UE
# CBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9z
# b2Z0IENvcnBvcmF0aW9uMSYwJAYDVQQDEx1NaWNyb3NvZnQgVGltZS1TdGFtcCBQ
# Q0EgMjAxMDAeFw0xODA4MjMyMDI2NDVaFw0xOTExMjMyMDI2NDVaMIHOMQswCQYD
# VQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEe
# MBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSkwJwYDVQQLEyBNaWNyb3Nv
# ZnQgT3BlcmF0aW9ucyBQdWVydG8gUmljbzEmMCQGA1UECxMdVGhhbGVzIFRTUyBF
# U046MTQ4Qy1DNEI5LTIwNjYxJTAjBgNVBAMTHE1pY3Jvc29mdCBUaW1lLVN0YW1w
# IFNlcnZpY2UwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDCFAkrrhfx
# NIIy/O6J8HbudDNQcFMUMLa2VhEGApizIRoTUf93kMQQ/ZamOBWMYUWdW8X7CI7O
# 0ir3BdJtWH4YCuY/QXWb87nnnZBGPDFVLuNcwdfHZ/jZmnpc+bxc/QS6uYSTwBHW
# YN2UfuHT+9fbVVyT/4lZLJS00yh/2Kk/FXInplzlGgrCv0Xu8XQXvKgUobDsG4A+
# 8VWMIJnr8/m0md1LFgpXHAQfZDITR4Z4iCpLjKOATSebXvsqfCgVcs6J7SprToUt
# VyjUiMsBFgM0RimbZT9+g1RjsvUhmo8HldEYx5KmTFA9bRIiGg6AaYmV2nBz53tR
# UBsEoyuDTJRLAgMBAAGjggEbMIIBFzAdBgNVHQ4EFgQUKWijMZR2Q9lfXQ2bn9Oy
# hPZiN4QwHwYDVR0jBBgwFoAU1WM6XIoxkPNDe3xGG8UzaFqFbVUwVgYDVR0fBE8w
# TTBLoEmgR4ZFaHR0cDovL2NybC5taWNyb3NvZnQuY29tL3BraS9jcmwvcHJvZHVj
# dHMvTWljVGltU3RhUENBXzIwMTAtMDctMDEuY3JsMFoGCCsGAQUFBwEBBE4wTDBK
# BggrBgEFBQcwAoY+aHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraS9jZXJ0cy9N
# aWNUaW1TdGFQQ0FfMjAxMC0wNy0wMS5jcnQwDAYDVR0TAQH/BAIwADATBgNVHSUE
# DDAKBggrBgEFBQcDCDANBgkqhkiG9w0BAQsFAAOCAQEAednLwunE0cCrLQ7SJr/a
# Z6AL/fbAtsEC8uK5gc1i9pCP7JXgTV6O+HKnQFqGHy0QkKPg7ltpuf9JjHuA6bzw
# y2AkEkiZALbv4VC/NMIpOWamjbyH6e4dfwifyjLiIHfFm4TWcS8huUuFuwLOuu0D
# Yn8Hgq4xamTFfV7QTtsI8/ULYtUY3TiJoV+1eJHQmCkrXUINsSYVFPWbT6hqa7YW
# j6raPRYXqrN+wsNrzt7QI2KjF02Qr8m8/uG1B8SFr6rySFy6bjz89cwrwqLS3wc8
# wYlfZoiBAV7J79qiCJHQ5yEqpayGdZ13UR3V5eGYQX98AGUfxMCC7ympLChWM65O
# CzCCBnEwggRZoAMCAQICCmEJgSoAAAAAAAIwDQYJKoZIhvcNAQELBQAwgYgxCzAJ
# BgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25k
# MR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xMjAwBgNVBAMTKU1pY3Jv
# c29mdCBSb290IENlcnRpZmljYXRlIEF1dGhvcml0eSAyMDEwMB4XDTEwMDcwMTIx
# MzY1NVoXDTI1MDcwMTIxNDY1NVowfDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldh
# c2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBD
# b3Jwb3JhdGlvbjEmMCQGA1UEAxMdTWljcm9zb2Z0IFRpbWUtU3RhbXAgUENBIDIw
# MTAwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCpHQ28dxGKOiDs/BOX
# 9fp/aZRrdFQQ1aUKAIKF++18aEssX8XD5WHCdrc+Zitb8BVTJwQxH0EbGpUdzgkT
# jnxhMFmxMEQP8WCIhFRDDNdNuDgIs0Ldk6zWczBXJoKjRQ3Q6vVHgc2/JGAyWGBG
# 8lhHhjKEHnRhZ5FfgVSxz5NMksHEpl3RYRNuKMYa+YaAu99h/EbBJx0kZxJyGiGK
# r0tkiVBisV39dx898Fd1rL2KQk1AUdEPnAY+Z3/1ZsADlkR+79BL/W7lmsqxqPJ6
# Kgox8NpOBpG2iAg16HgcsOmZzTznL0S6p/TcZL2kAcEgCZN4zfy8wMlEXV4WnAEF
# TyJNAgMBAAGjggHmMIIB4jAQBgkrBgEEAYI3FQEEAwIBADAdBgNVHQ4EFgQU1WM6
# XIoxkPNDe3xGG8UzaFqFbVUwGQYJKwYBBAGCNxQCBAweCgBTAHUAYgBDAEEwCwYD
# VR0PBAQDAgGGMA8GA1UdEwEB/wQFMAMBAf8wHwYDVR0jBBgwFoAU1fZWy4/oolxi
# aNE9lJBb186aGMQwVgYDVR0fBE8wTTBLoEmgR4ZFaHR0cDovL2NybC5taWNyb3Nv
# ZnQuY29tL3BraS9jcmwvcHJvZHVjdHMvTWljUm9vQ2VyQXV0XzIwMTAtMDYtMjMu
# Y3JsMFoGCCsGAQUFBwEBBE4wTDBKBggrBgEFBQcwAoY+aHR0cDovL3d3dy5taWNy
# b3NvZnQuY29tL3BraS9jZXJ0cy9NaWNSb29DZXJBdXRfMjAxMC0wNi0yMy5jcnQw
# gaAGA1UdIAEB/wSBlTCBkjCBjwYJKwYBBAGCNy4DMIGBMD0GCCsGAQUFBwIBFjFo
# dHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vUEtJL2RvY3MvQ1BTL2RlZmF1bHQuaHRt
# MEAGCCsGAQUFBwICMDQeMiAdAEwAZQBnAGEAbABfAFAAbwBsAGkAYwB5AF8AUwB0
# AGEAdABlAG0AZQBuAHQALiAdMA0GCSqGSIb3DQEBCwUAA4ICAQAH5ohRDeLG4Jg/
# gXEDPZ2joSFvs+umzPUxvs8F4qn++ldtGTCzwsVmyWrf9efweL3HqJ4l4/m87WtU
# VwgrUYJEEvu5U4zM9GASinbMQEBBm9xcF/9c+V4XNZgkVkt070IQyK+/f8Z/8jd9
# Wj8c8pl5SpFSAK84Dxf1L3mBZdmptWvkx872ynoAb0swRCQiPM/tA6WWj1kpvLb9
# BOFwnzJKJ/1Vry/+tuWOM7tiX5rbV0Dp8c6ZZpCM/2pif93FSguRJuI57BlKcWOd
# eyFtw5yjojz6f32WapB4pm3S4Zz5Hfw42JT0xqUKloakvZ4argRCg7i1gJsiOCC1
# JeVk7Pf0v35jWSUPei45V3aicaoGig+JFrphpxHLmtgOR5qAxdDNp9DvfYPw4Ttx
# Cd9ddJgiCGHasFAeb73x4QDf5zEHpJM692VHeOj4qEir995yfmFrb3epgcunCaw5
# u+zGy9iCtHLNHfS4hQEegPsbiSpUObJb2sgNVZl6h3M7COaYLeqN4DMuEin1wC9U
# JyH3yKxO2ii4sanblrKnQqLJzxlBTeCG+SqaoxFmMNO7dDJL32N79ZmKLxvHIa9Z
# ta7cRDyXUHHXodLFVeNp3lfB0d4wwP3M5k37Db9dT+mdHhk4L7zPWAUu7w2gUDXa
# 7wknHNWzfjUeCLraNtvTX4/edIhJEqGCA60wggKVAgEBMIH+oYHUpIHRMIHOMQsw
# CQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
# ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSkwJwYDVQQLEyBNaWNy
# b3NvZnQgT3BlcmF0aW9ucyBQdWVydG8gUmljbzEmMCQGA1UECxMdVGhhbGVzIFRT
# UyBFU046MTQ4Qy1DNEI5LTIwNjYxJTAjBgNVBAMTHE1pY3Jvc29mdCBUaW1lLVN0
# YW1wIFNlcnZpY2WiJQoBATAJBgUrDgMCGgUAAxUArcMkvNHZdfBd0N1FHzkxnm7W
# nrGggd4wgdukgdgwgdUxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9u
# MRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRp
# b24xKTAnBgNVBAsTIE1pY3Jvc29mdCBPcGVyYXRpb25zIFB1ZXJ0byBSaWNvMScw
# JQYDVQQLEx5uQ2lwaGVyIE5UUyBFU046NTdGNi1DMUUwLTU1NEMxKzApBgNVBAMT
# Ik1pY3Jvc29mdCBUaW1lIFNvdXJjZSBNYXN0ZXIgQ2xvY2swDQYJKoZIhvcNAQEF
# BQACBQDfRw+zMCIYDzIwMTgwOTE1MTI1NzIzWhgPMjAxODA5MTYxMjU3MjNaMHQw
# OgYKKwYBBAGEWQoEATEsMCowCgIFAN9HD7MCAQAwBwIBAAICF40wBwIBAAICGGcw
# CgIFAN9IYTMCAQAwNgYKKwYBBAGEWQoEAjEoMCYwDAYKKwYBBAGEWQoDAaAKMAgC
# AQACAwehIKEKMAgCAQACAwehIDANBgkqhkiG9w0BAQUFAAOCAQEAKM80Cx7ww6Fv
# moppfAui0HSW15re4MtgwUnu75STkH5hpYfZxsakYyhoZjatIvV6ap35FUjc1JGV
# 4+NsFNe3uDI3LLMja/jiuqYdvB4eedb6b9bBY+/6YfZNkccyD9nT1FjGkjO7UAC+
# NKE1AUMbmBMYilMY2Cj/qrVagWe/twdYjPwCwS+JZZD289pjp2SClZE1a7pwf3NB
# aGntaC3dSHPI0H30/UBuB9BcKWn4kOVdDHFygcwWVzMqufLRQ1xBxhH2JL3lDaSn
# HW82VRbJ6pl8eNIesCXdnijyk0R7tz0Fg5b8yPg77BU3JvBrsCJFtSitvot6THR6
# MJl/ox/bKjGCAvUwggLxAgEBMIGTMHwxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpX
# YXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQg
# Q29ycG9yYXRpb24xJjAkBgNVBAMTHU1pY3Jvc29mdCBUaW1lLVN0YW1wIFBDQSAy
# MDEwAhMzAAAA1acj5XiVagn/AAAAAADVMA0GCWCGSAFlAwQCAQUAoIIBMjAaBgkq
# hkiG9w0BCQMxDQYLKoZIhvcNAQkQAQQwLwYJKoZIhvcNAQkEMSIEIGqRZ71BlprB
# K0+Ymvnv0WFz+gd1EkcFDosEmn6NvHCVMIHiBgsqhkiG9w0BCRACDDGB0jCBzzCB
# zDCBsQQUrcMkvNHZdfBd0N1FHzkxnm7WnrEwgZgwgYCkfjB8MQswCQYDVQQGEwJV
# UzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UE
# ChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSYwJAYDVQQDEx1NaWNyb3NvZnQgVGlt
# ZS1TdGFtcCBQQ0EgMjAxMAITMwAAANWnI+V4lWoJ/wAAAAAA1TAWBBS/pE0Dn2KK
# LQsMKImN7mWWXhHGhzANBgkqhkiG9w0BAQsFAASCAQALRNyPMFXeZm0KGjNQ39JJ
# CfdH2x46pGvTewienntf/6IloOhz+feRShoFhB0h2NZaw2yZmvA2n/8haI7lvxh7
# Usb1kkR1TbIWFueo+E+WoSmnTdpzCmvjryF46S+g+ySsCNnoScyJ7NpAoXTZX2nL
# 6D1ARGGmKNoYQm5DZUcvCKL/x3AFDuPLivOCTorJAOa9NTzM+Dxaxj5jGdakytOK
# LP6ORc/vJvZHc2CtZTlTQveCmEop/p78LZ7U8CfAidW5sz2fDyGrg5cvF3AmmVZn
# wUEtXJ4E9W5KhjvGXQmF1yuHynBimPi9NO6COAKkLwBwyt9Ehe7/XMl8MN4e2ZZj
# SIG # End signature block
