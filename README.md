# TP_IMD

## Contrucción del sistema

Se baso en el sistema implementado para iso2, donde:
+ Se realizo la construcción de Toolchain para trabajar - Compilación cruzada CrossTools-NG
+ Se utilizo uboot como cargador de arranque. Se preparo una memoria microSD para poder utilizarla como unidad de booteo.
+ Se utiliza un adaptador USB-UART TTL conectado a la placa, para poder ver la consola de salida del bootloader.
+ Se compila el  kernel de linux a utilizar, se generan dos archivos: zImage (imagen kernel) y am335x-boneblack.dtb (device tree).
+ Se cargan los archivos del kernel mediante el servidor TFTP.
+ Se utiliza busybox y un filesystem del tipo NFS.


## Hardware

### Beaglebone black

![sistema](../img/beaglebone.png)

### Esclavo i2c

Como esclavo i2c se utiliza el módulo i2c BMP280. 
![BMP280](../img/bmp280.png)

Este tiene dos posibles direcciones, si se conecta SDO a GND entonces la direccion del esclavo i2c es 0x76.

Se realiza la lectura de los siguientes registros, para obtener la medida de la temperatura.
![reg](../img/reg_tmp.png)


## Implementación del Driver I2C

