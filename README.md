This tool uninstalls MBRFilterPP 

Steps: 
1.Download the executable from here or compile it yourself, then place it in a easy path say: 
C:\MBRUnins.exe 

2. Boot to WinRE then go to Troubleshoot > Advanced Options > Startup settings > Restart > Safe Mode (Minimal or with networking doesn't matter)
or you can do it Via bcdedit.exe with:

bcdedit /set {current} safeboot minimal

or you can boot with networking if you need to download the tool

bcdedit /set {current} safeboot network

3. Open cmd.exe and type in: 
X:\Path\to\executable.exe

few notes which are:

1. Change the Drive letter X: to where the Acutal executable lives

2. you need the full path to executable say i have it in Users file 
i would need to type in: C:\Users\MBRUnins.exe

that is why i recommend just placing it in the root of C:\ drive for easier access, Oh don't forget to place the actual .exe

extension and not just C:\MBRUnins (for example)

4. after running the tool successfuly (Which only runs in safe mode)
open regedit.exe and go to:
HKLM\SYSTEM\ControlSet001\Services  AND ITS SYMBOLIC LINK:    HKLM\SYSTEM\CurrentControlSet\Services

and make sure that MBRFilterPP doesn't exist afterwards, 

go to: 
HKLM\SYSTEM\ControlSet001\Control\Class\{4d36e967-e325-11ce-bfc1-08002be10318}

and make sure of UpperFilters value it should be only: partmgr, AND NOT: partmgr MBRFilterPP

MAKE SURE that the value partmgr is ONLY partmgr without spaces and make sure of the value itself IT SHOULD NOT be empty at all

Edge cases: 
you may have other filters installed as well while this could cause issues but if so 

make sure the value is 
partmgr
(and other filters)
MBRFilterPP SHOULD NOT be in that list or value


5. after making sure the service and its components are removed properly you can reboot and you should be fine
