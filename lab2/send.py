from serial import Serial
import struct
import argparse
import time
import sys

def send(args):
    try:
        print(f"try to open {args.port}...")
        with Serial(args.port, args.baudrate, timeout=args.timeout) as ser:
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
                    
                    # 等待bootloader準備好
                    print("Waiting for bootloader to be ready...")
                    time.sleep(1)
                    
                    # send header
                    print("Sending header...")
                    header = struct.pack('<I', size)
                    ser.write(header)
                    ser.flush() # wait for all data to be sent
                    
                    print("Header sent, waiting before sending kernel data...")
                    time.sleep(0.5)  # 在發送頭和數據之間添加短暫延遲
                    
                    # 分批發送數據以避免緩衝區溢出
                    print("Sending kernel data...")
                    chunk_size = 256  # 每次發送256字節
                    sent = 0
                    
                    for i in range(0, len(kernel_data), chunk_size):
                        chunk = kernel_data[i:i+chunk_size]
                        ser.write(chunk)
                        ser.flush()
                        sent += len(chunk)
                        print(f"Sent {sent}/{size} bytes", end="\r")
                        time.sleep(0.01)  # 每個數據包之間添加小延遲
                    
                    print("\nKernel data sent! Waiting for confirmation...")
                    time.sleep(2)  # 等待bootloader處理完數據
                    
                    # 輸出接收到的響應
                    if ser.in_waiting:
                        response = ser.read(ser.in_waiting).decode(errors='ignore')
                        print(f"Response from bootloader: {response}")
                    
                    print("\nTransfer complete!")

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
    parser.add_argument('--timeout', '-t', type=float,
                       help="serial port timeout in seconds",
                       default=5.0)
    args = parser.parse_args()

    send(args)