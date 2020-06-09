# CAN-RS485-adapter

HW device in shop:
https://shop.emkit.net/ru_RU/shop/product/rs-485-can-21

For first use:
- Clone repository to local folder
- Run SW4STM32 (System Workbench for STM32)
- Select "CAN-RS485-adapter/Firmware" workspace directory
- Close "Welcome" window
- In "Project explorer" via context menu call "Import/General/Exsting Project into Workspace" and select "CAN-RS485-adapter/Firmware/A11096" directory
- After first "Debug" command select "Ac6 STM32 C/C++ Application" 




For taking current, voltage or velocity from can-adapter you need detach other adapters. And select current, voltage, velocity sources from RS485.

If you want do not use current sensor on CAN-adapter (for example), comment _ProtocolHost->AddSelfAddr(EmkAddr::CurrentSensor);

void Kernel::Init()
{
	_ProtocolHost = new ProtocolHost(EmkAddr::SpeedSensor);
	_ProtocolHost->AddSelfAddr(EmkAddr::CurrentSensor);
	_ProtocolHost->AddSelfAddr(EmkAddr::VoltageSensor);
.....
}
