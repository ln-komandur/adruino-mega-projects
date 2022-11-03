# Projects done using Arduino Mega2560 kit
### Refer respective source code files

# Installing Eclipse CPP IDE on Linux

#### Reference - [Install Eclipse IDE with CDT plugin for C/C++ coding on Linux](https://www.pragmaticlinux.com/2021/01/install-eclipse-ide-with-cdt-plugin-for-c-c-coding-on-linux/) 

`sudo apt install openjdk-14-jre-headless`
  
`tar xvf $HOME/Downloads/eclipse-cpp-2020-12-R-linux-gtk-x86_64.tar.gz -C $HOME`

`mv $HOME/eclipse $HOME/eclipse-installation`

`sudo ln -s $HOME/eclipse-installation/eclipse /usr/local/bin/`

`eclipse`

`sudo nano /usr/share/applications/eclipse.desktop`

### Ports

If you connect Arduino Mega, then it will be listed as `/dev/ttyACM0` . Check it by listing `ls -l /dev/ttyACM0` . 

If you connect ESP32, it will be listed as `/dev/ttyUSB0` . Check it by listing `ls -l /dev/ttyUSB0` . 

#### Refer [What is the difference between devttyUSBx and devttyACMx?](https://rfc1149.net/blog/2013/03/05/what-is-the-difference-between-devttyusbx-and-devttyacmx/) 


`sudo usermod -a -G dialout $USER` . Refer [Adding yourself to the dialout group](https://askubuntu.com/questions/58119/changing-permissions-on-serial-port)

`reboot` (or logout and login)

### Install Sloeber, the Arduino Plugin for Eclipse from Eclipse Marketplace

### Installing ESP-IDF 

#### References
1.  [ESP-IDF Eclipse Plugin](https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md)
1.  [Getting Started with the Espressif esp-idf & Eclipse IDE](https://www.beyondlogic.org/getting-started-with-the-espressif-esp-idf-eclipse-ide/)
1.  [Programming ESP32 using ESP-IDF for TensorFlow Lite](https://medium.com/@prabhakarpanday/programming-esp32-using-esp-idf-for-tensorflow-lite-f173eec91c01)
1.  [ESP32-DevKitC V4 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html)
1.  [ESP-IDF Releases](https://github.com/espressif/esp-idf/releases) which has commands like `python3 -version` , 
`git clone -b v4.2 --recursive https://github.com/espressif/esp-idf.git esp-idf-v4.2` , `which git` , `which python3` , `whereis python3`
