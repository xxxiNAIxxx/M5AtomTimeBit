[Russian](README.md) | English(README_Eng.md)
  
# M5AtomTimeBit
## Description  
Application for [M5Atom Matrix](https://m5stack.com/collections/m5-atom/products/atom-matrix-esp32-development-kit).  
It's used for starts (closes, pauses) new workday in [bitrix24](https://www.bitrix24.com/). Short press - pause. Long press - open\close workday.  
  
<img src="/image/gif.gif"/> 

## Сonfiguration  
Rename the Settings-example to Settings.ino.  
```serialSpeed = 115200;``` - speed for debug messages  
```updateTimeInterval=10;``` - update interval =)  
```brightness=50;``` - brightness  
  
```wifiSsid     = "WiFiName"``` - network name  
```wifiPassword = "WiFiPassword";``` - password  
  
```urnBase="https://company.bitrix24.ru/rest/01/1234567890qwertyu/";``` - link to the REST API (см. [FAQ](https://github.com/xxxiNAIxxx/M5AtomTimeBit/wiki/FAQ))  
```openReason="Best day of my life";``` - Reason to modify workday time(open). Required when specifying TIME parameter and with disabled flexible hours for a user.  
```closeReason="This is the end";``` - Reason to modify workday time(close).  
    
```urnStatus, urnOpen, urnClose, urnPause``` - [API](https://training.bitrix24.com/rest_help/time_management/basic/index.php) function  
```rootCa``` - ssl certificate
  
File image_all contains four image (5х5).  
<img src="/image/image_all_scr.png" alt="drawing" width="300"/>  
  
Use [ATOOOOM Pixel TOOL](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/software/AtomPixTool.exe) to view and edit.

## Additional libs and advanced options  
*FastLED v3.3.3  
*M5Atom v0.0.1  
*M5SticC v0.2.0  
*M5Stack v0.3.0  
*ArdunioJSON v6.16.1  
  
**IMPOTANT!** Update M5Atom lib from [repo](https://github.com/m5stack/M5Atom), version intalled from Library Manager don't work correctly!  
  
Using Wifi and FastLED leads to flickering and flashing random LED (see [issue](https://github.com/m5stack/M5Atom/issues/15)).  
Solution: In ```M5Atom.cpp``` (...\Documents\Arduino\libraries\M5Atom\src\) add ```dis.setCore(1);``` inside ```if( DisplayEnable )```:  
```
if( DisplayEnable )
{
    	dis.setTaskName("LEDs");
	dis.setTaskPriority(2);
	dis.setCore(1);
	dis.start();
}
```
