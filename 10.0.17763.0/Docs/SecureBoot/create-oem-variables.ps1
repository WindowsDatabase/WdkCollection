param($pkFile = $(throw "pkFile parameter is required"),
    $kekFiles = $(throw "kekFiles parameter is required"),
    $dbFiles = $(throw "dbFiles parameter is required"),
    $ownerGuid = $(throw "ownerGuid parmeter is required")
    )

# adapted from subcreate_outofbox_example.ps1 in win8 tree

# example usage
# create-oem-variables -pk msftTestRoot2010.cer -kek msftTestRoot2010.cer -db ("MsftTestRoot2010.cer", "windowsPhoneProd2012Pca.cer") -ownerGuid "55555555-5555-5555-5555-555555555555"

# for creating variable blobs containing x509 certificates 
function format-sb($var, $sigOwner, $certPath)
{
    #hardcoded timestamp for setupmode
    $timestamp = "2011-05-21T13:30:00Z"

    $prefix = $var
    
    $siglist = $prefix + ".bin"
    $setVariableOutput = $prefix + "_setVariable.bin"
    
    Format-SecureBootUEFI -Name $var -SignatureOwner $sigowner -ContentFilePath $siglist -FormatWithCert `
        -Certificate $certpath  -Time $timestamp | out-null

    Set-SecureBootUEFI -Name $var -Time $timestamp -ContentFilePath $siglist `
         -OutputFilePath $setVariableOutput | out-null

    # we only used the siglist to create the _setvariable file, clean it up now
    del $siglist
}


# for creating variable updates containing sha256 hashes (for DBX)
function format-sb-hashes($var, $sigOwner, $hashes)
{
    #hardcoded timestamp for setupmode
    $timestamp = "2011-05-21T13:30:00Z"

    $prefix = $var
    
    $siglist =  $prefix + ".bin"
    $setVariableOutput = $prefix + "_setVariable.bin"
    
    Format-SecureBootUEFI -Name $var -SignatureOwner $sigowner -ContentFilePath $siglist  `
        -Hash $hashes -algorithm sha256 -Time $timestamp | out-null

    Set-SecureBootUEFI -Name $var -Time $timestamp -ContentFilePath $siglist `
         -OutputFilePath $setVariableOutput | out-null

    # we only used the siglist to create the _setvariable file, clean it up now
    del $siglist
}

set-strictmode -version latest

write-progress -activity "Making secure boot variables" -status "Creating PK"
format-sb "PK" $ownerGuid $pkFile

write-progress -activity "Making secure boot variables" -status "Creating KEK"
format-sb "KEK" $ownerGuid $kekFiles

write-progress -activity "Making secure boot variables" -status "Creating DB"
format-sb "db" $ownerGuid $dbFiles

write-progress -activity "Making secure boot variables" -status "Creating DBX"


# add a SHA256 hash of "0" to DBX so that DBX is not empty
$hashObject = new-object System.Security.Cryptography.SHA256Managed
$encodeObject = new-object System.Text.ASCIIEncoding
$hashString = ([System.BitConverter]::ToString($hashObject.ComputeHash(0))).Replace("-","")
[string[]] $hashes = @()
$hashes += $hashString
format-sb-hashes "dbx" $ownerGuid $hashes

