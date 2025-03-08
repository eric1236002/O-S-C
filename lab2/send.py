from serial import Serial
import struct
import argparse
import time
import sys

def send(args):
    try:
        print(f"try to open {args.port}...")
        with Serial(args.port, args.baudrate, timeout=1) as ser:
            print(f"port opened: {ser.name}")
            
            print(f"read {args.kernel_path}...")
            with open(args.kernel_path, "rb") as f:
                try:
                    kernel_data = f.read()
                    size = len(kernel_data)
                    
                    print(f"file size: {size} bytes")

                    # clear buffer
                    ser.reset_input_buffer()
                    ser.reset_output_buffer()
                    
                    # send header
                    header = struct.pack('<I', size)
                    ser.write(header)
                    ser.flush() # wait for all data to be sent
                    
                    print("\n header send !")
                    ser.write(kernel_data)
                    ser.flush() # wait for all data to be sent
                    
                    
                    print("\n kernal send !")

                except Exception as e:
                    print(f"file error: {e}")
                    sys.exit(1)
    except Exception as e:
        print(f"error: {e}")
        sys.exit(1)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Send kernel image to Raspberry Pi')
    parser.add_argument('--port', '-p', type=str, 
                       help="port (Windows: COM1, Linux: /dev/ttyUSB0)", 
                       default="/dev/pts/0")
    parser.add_argument('--baudrate', '-b', type=int, 
                       help="baudrate", 
                       default=115200)
    parser.add_argument('--kernel-path', '-k', type=str, 
                       help="path to kernel image file", 
                       default="./kernel8.img")
    args = parser.parse_args()

    send(args)