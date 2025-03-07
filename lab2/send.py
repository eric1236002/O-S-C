from serial import Serial
import struct
import argparse
import time


def send(args):
    with Serial(args.serial_port, args.baudrate) as ser:
        with open(kernal_path, "rb") as f:
            try:
                kernel_data = f.read()
                checksum = 0
                for byte in kernel_data:
                    checksum += byte
                checksum = checksum & 0xFFFFFFFF
                print("Kernal file read")

                # Send the size of the kernal file
                header= struct.pack('<II', 
                    len(kernel_data),     # size
                    checksum             # checksum
                )
                ser.write(header)
                time.sleep(0.1)
                ser.write(kernel_data)
                time.sleep(0.1) 
                print("Kernal file sent")


            except Exception as e:
                print(e)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('port', type=str,help="Serial port to send data to",default="/dev/ttyUSB0")
    parser.add_argument('baudrate', type=int,help="Baudrate of the serial port",default=115200)
    parser.add_argument("-Kernal_path", type=str,help="Path to the kernal file",default="./kernal8.img")
    args = parser.parse_args()

    send(args)