LargeAddressToggle
==================

This tool enables or disables the `IMAGE_FILE_LARGE_ADDRESS_AWARE` flag on PE files. Use at your own risk. Things will break... Probably.  
This tool doesn't check if the file has a valid PE header.

## Parameters

`LargeAddressToggle <show|enable|disable> <path>`

* `show`: Shows architecture and if the `IMAGE_FILE_LARGE_ADDRESS_AWARE` flag is present;
* `enable`: Enables the `IMAGE_FILE_LARGE_ADDRESS_AWARE` flag;
* `disable`: Disables the `IMAGE_FILE_LARGE_ADDRESS_AWARE` flag.

Then you specify the path to `.exe` you wish to patch.
