SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\ProgramData\Oracle\Java\javapath;C:\Program Files (x86)\Intel\iCLS Client\;C:\Program Files\Intel\iCLS Client\;C:\windows\system32;C:\windows;C:\windows\System32\Wbem;C:\windows\System32\WindowsPowerShell\v1.0\;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\IPT;C:\Program Files\Intel\Intel(R) Management Engine Components\IPT;C:\Program Files\MATLAB\R2016b\bin
SET CPU_TYPE=STM32F407VGTx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=rt-thread_stm32f4xx
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\uart_protocol.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\uart_device.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\uart_parameters.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\build\rtthread-stm32f4xx.lnp"
fromelf --bin .\build\rtthread-stm32f4xx.axf --output rtthread.bin
