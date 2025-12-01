# File and FTP details
$watchFile = "$PSScriptRoot\.pio\build\esp32dev\VSense_V17.bin"  # Path to the file you want to monitor
$ftpServer = "ftp://orlang.hu/public_html/vsense"
$ftpUsername = "orlanghu"
$ftpPassword = "R4v4r1r0m!"
$lastModifiedFile = "$PSScriptRoot\.pio\build\esp32dev\lastModifiedDate.txt"  # File to store the last modified date

# Initialize the last modified date
if (Test-Path $lastModifiedFile) {
    $lastModifiedDate = Get-Content $lastModifiedFile | Out-String | ConvertFrom-StringData
} else {
    $lastModifiedDate = @{ "LastModified" = [DateTime]::MinValue }
}

while ($true) {
    Start-Sleep -Seconds 10  # Check every 10 seconds

    if (Test-Path $watchFile) {
        $currentLastModified = (Get-Item $watchFile).LastWriteTime

        if ($currentLastModified -ne $lastModifiedDate.LastModified) {
            Write-Host " "
            Write-Host "File has been modified."
            Get-Date            
            Write-Host "Starting upload..."

            try {
                # Create the FTP request
                Write-Host "Creating FTP Request..."
                Write-Host "FTP Server: "                
                Write-Host "$ftpServer/$(Split-Path -leaf $watchFile)"
                $ftpRequest = [System.Net.FtpWebRequest]::Create("$ftpServer/$(Split-Path -leaf $watchFile)")
                $ftpRequest.Method = [System.Net.WebRequestMethods+Ftp]::UploadFile
                $ftpRequest.Credentials = New-Object System.Net.NetworkCredential($ftpUsername, $ftpPassword)
                $ftpRequest.UsePassive = $true
                $ftpRequest.KeepAlive = $false
                $ftpRequest.UseBinary = $true

                # Read the file data into a byte array
                $fileContents = [System.IO.File]::ReadAllBytes($watchFile)
                $ftpRequest.ContentLength = $fileContents.Length

                # Upload the file
                $requestStream = $ftpRequest.GetRequestStream()
                $requestStream.Write($fileContents, 0, $fileContents.Length)
                $requestStream.Close()

                # Get the response from the FTP server
                $ftpResponse = $ftpRequest.GetResponse()
                Write-Host "File Uploaded to FTP successfully."
                $ftpResponse.Close()

                # Update last modified date record
                Write-Host "Updating last modified date..."
                $lastModifiedDate = @{ "LastModified" = $currentLastModified }
                $lastModifiedDateString = "LastModified=$currentLastModified"
                Set-Content -Path $lastModifiedFile -Value $lastModifiedDateString
                Write-Host "Last date modified Successfully!"

            } catch {
                Write-Host "FTP upload failed: $_"
            }
        } else {
            # Write-Host "No change detected. Skipping upload."
        }
    } else {
        Write-Host "File does not exist: $watchFile"
    }
}