This tool uninstalls MBRFilterPP 

USAGE: 
1-Open CMD in safe mode 

2-run: C:\Path to the Executable

and it will run to remove MBRFilterPP

Before rebooting Check the services key: 

Computer\HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services

or 

Computer\HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Services

make sure it is not there.
Then go to: 
Computer\HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Class\{4d36e967-e325-11ce-bfc1-08002be10318}

and make sure that the UpperFilters key exist with the value being PartMgr 
if 
The MBRFilterPP entry doesn't exist in both 
Then reboot and it should be removed with no issues
