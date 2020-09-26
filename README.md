Russian | [English](README_Eng.md)
  
# M5AtomTimeBit
## Описание  
Приложение для [M5Atom Matrix](https://m5stack.com/collections/m5-atom/products/atom-matrix-esp32-development-kit).  
Позволяет управлять рабочим днем в [bitrix24](https://www.bitrix24.ru/) - начинать, заканчивать, ставить на паузу. Короткое нажатие включает\отключает перерыв. Длительное открывает\закрывает рабочий день.  
  
<img src="/image/gif.gif"/> 

## Настройки  
Файл Settings-example переименовать в Settings.ino.  
```serialSpeed = 115200;``` - скорость для сервисных и debug сообщений  
```updateTimeInterval=10;``` - интервал обновления данных  
```brightness=50;``` - яркость LED-панели  
  
```wifiSsid     = "WiFiName"``` - указать имя беспроводной сети  
```wifiPassword = "WiFiPassword";``` - указать пароль беспроводной сети  
  
```urnBase="https://company.bitrix24.ru/rest/01/1234567890qwertyu/";``` - путь к REST API, указать адрес сайта (см. [FAQ](https://github.com/xxxiNAIxxx/M5AtomTimeBit/wiki/FAQ))  
```openReason="Best day of my life";``` - причина начала рабочего дня  
```closeReason="This is the end";``` - причина завершения рабочего дня  
    
```urnStatus, urnOpen, urnClose, urnPause``` - функции [API](https://dev.1c-bitrix.ru/rest_help/timeman/base/index.php)  
```rootCa``` - сертификат УЦ битрикса
  
Файл image_all содержит четыре иконки (5х5).  
<img src="/image/image_all_scr.png" alt="drawing" width="300"/>  
  
Для просмотра и редактирования можно воспользоваться [ATOOOOM Pixel TOOL](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/software/AtomPixTool.exe)

## Необходимые библиотеки и дополнительные настройки  
*FastLED v3.3.3  
*M5Atom v0.0.1  
*M5SticC v0.2.0  
*M5Stack v0.3.0  
*ArdunioJSON v6.16.1  
  
**ВАЖНО!** M5Atom необходимо обновить из [репозитория](https://github.com/m5stack/M5Atom), версия устанавливаемая через Library Manager не работает!  
  
При использовании Wi-Fi и FastLed могут наблюдаться мерцания и вспышки отдельных светодиодов (см. [тут](https://github.com/m5stack/M5Atom/issues/15)).  
Решение: в файле ```M5Atom.cpp``` (...\Documents\Arduino\libraries\M5Atom\src\) необходимо добвать ```dis.setCore(1);``` в секцию ```if( DisplayEnable )```:  
```
if( DisplayEnable )
{
    	dis.setTaskName("LEDs");
	dis.setTaskPriority(2);
	dis.setCore(1);
	dis.start();
}
```
