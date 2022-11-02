# Projects done using Arduino Mega2560 kit


### Installing Eclipse CPP IDE on Linux

Reference - https://www.pragmaticlinux.com/2021/01/install-eclipse-ide-with-cdt-plugin-for-c-c-coding-on-linux/ 

`sudo apt install openjdk-14-jre-headless`
  
`tar xvf $HOME/Downloads/eclipse-cpp-2020-12-R-linux-gtk-x86_64.tar.gz -C $HOME`

`mv $HOME/eclipse $HOME/eclipse-installation`

`sudo ln -s $HOME/eclipse-installation/eclipse /usr/local/bin/`

`eclipse`

`sudo nano /usr/share/applications/eclipse.desktop`

### Ports

If you connect Arduino Mega, then it will be listed as `/dev/ttyACM0` . Check it by listing `ls -l /dev/ttyACM0` . 

If you connect ESP32, it will be listed as `/dev/ttyUSB0` . Check it by listing `ls -l /dev/ttyUSB0` . 

Refer [What is the difference between devttyUSBx and devttyACMx?](https://rfc1149.net/blog/2013/03/05/what-is-the-difference-between-devttyusbx-and-devttyacmx/) 


`sudo usermod -a -G dialout $USER` . Refer [Adding yourself to the dialout group](https://askubuntu.com/questions/58119/changing-permissions-on-serial-port)

`reboot` (or logout and login)

### Install Sloeber, the Arduino Plugin for Eclipse from Eclipse Marketplace

### Installing ESP-IDF 

Reference - https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md


`python3 -version`

`git clone -b v4.2 --recursive https://github.com/espressif/esp-idf.git esp-idf-v4.2`

`which git`

`which python3`

`whereis python3`

