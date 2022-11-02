# Projects done using Arduino Mega2560 kit


### Installing Eclipse CPP IDE on Linux

Reference - https://www.pragmaticlinux.com/2021/01/install-eclipse-ide-with-cdt-plugin-for-c-c-coding-on-linux/ 

`sudo apt install openjdk-14-jre-headless`
  
`tar xvf $HOME/Downloads/eclipse-cpp-2020-12-R-linux-gtk-x86_64.tar.gz -C $HOME`

`mv $HOME/eclipse $HOME/eclipse-installation`

`sudo ln -s $HOME/eclipse-installation/eclipse /usr/local/bin/`

`eclipse`

`sudo nano /usr/share/applications/eclipse.desktop`

`ls -l /dev/ttyACM0` 

`sudo usermod -a -G dialout $USER`

`reboot`

### Install Sloeber, the Arduino Plugin for Eclipse from Eclipse Marketplace

### Installing ESP-IDF 

Reference - https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md


`python3 -version`

`git clone -b v4.2 --recursive https://github.com/espressif/esp-idf.git esp-idf-v4.2`

`which git`

`which python3`

`whereis python3`

