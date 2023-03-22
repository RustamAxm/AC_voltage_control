# AC_voltage_control
## Description 
 Данное устройство предназначено для управления [ЛАТРом](https://www.chipdip.ru/product0/9000288674) с помощью шагового двигателя с редуктором. Модели для крепления в папке 3D моделей.
 Загрузка происходит стандартными средствами Arduino. 
 Если поключен [дисплей](https://market.yandex.ru/product--displei-gsmin-oled-0-96-128x64-i2c-sinii/1740294804?cpa=1) по I2C, то IP адресс будет выведен на него. В данной реализации выдется только по DHCP. Так же IP выводится в serial монитор: 
``` 
Initialize Ethernet with DHCP:
  DHCP assigned IP {current_ip}
```

По адресу доступна веб страничка на которой есть инструкция по установке напряжения на ЛАТРе.
Так же в репозитории имеется пример на python [ac_voltage_control.py](ac_voltage_control.py).

## Image
![](images/example.JPG?raw=true )
