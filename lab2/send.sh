sudo chmod 666 /dev/ttyUSB0
sudo minicom -b 115200 -o -D /dev/ttyUSB0

source /home/user/miniconda3/bin/activate base
python3 send.py --kernel-path ./kernel8.img --port /dev/ttyUSB0 --baudrate 115200 --timeout 5.0