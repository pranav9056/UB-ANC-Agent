Copyright © 2014, 2015, 2016 Jalil Modares

This program is part of my Ph.D. Dissertation research in the Department of Electrical Engineering at the University at Buffalo. I work in UB's Multimedia Communications and Systems Laboratory with my Ph.D. adviser, Prof. Nicholas Mastronarde <http://www.eng.buffalo.edu/~nmastron/>.

If you use this program for your work/research, please cite:
J. Modares, N. Mastronarde, M. J. Medley, J. D. Matyjas, "UB-ANC: An Open Platform Testbed for Software-Defined Airborne Networking and Communications" <http://arxiv.org/abs/1509.08346>.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

# UB-ANC
Open Testbed Platform for Drones

Raspberry Pi
------------

Run these commands to install Qt and build the agent:

```
sudo apt-get install qt5-default
sudo apt-get install libqt5serialport5-dev
git clone https://github.com/jmodares/UB-ANC-Agent
mkdir build-agent
cd build-agent
qmake ../UB-ANC-Agent
make -j4
```

USRP E310
----------

Steps needed for using serial port to connect to USRP E310:
  - Run `sudo adduser $USER dialout (screen /dev/ttyUSB0 115200)`
  - Logout
  - Run `screen /dev/ttyUSB0 115200`

To use USRP E310 folder on your host:
  - Run `sudo adduser $USER fuse`
  - Logout
  - Run `sshfs -o allow_root root@128.205.187.244:/ usrp-folder-host`

Steps needed to port Qt5 to USRP E310:

```
git clone  https://github.com/qtproject/qtbase
cd qtbase
git checkout stable
mkdir build
cd build
../configure {with custom parameters in the folder}
make -j4
make install
```

Steps needed to port QtSerialPort to USRP E310:

```
git clone  https://github.com/qtproject/qtserialport
cd qtserial
git checkout stable
mkdir build
cd build
/path-to-sdk/sysroots/armv7ahf-vfp-neon-oe-linux-gnueabi/usr/local/Qt-5.6.0/bin/qmake ..
make -j4
make install
```

Steps for Volk benchmark testing on USRP E310:

```
pip install numpy
pip install matplotlib
cd /path-to-gnuradio/examples/volk_benchmark
vim volk_test_funcs.py 
replace all scipy with numpy
read the README file for doing different tests.
```
