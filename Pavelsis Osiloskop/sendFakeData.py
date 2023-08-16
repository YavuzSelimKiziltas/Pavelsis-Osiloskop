import serial
import time
import random
import math

"""
This is a python file to generate fake data to send over UART

"""

# Seri port ayarları
serial_port = "COM11"  # Kullanacağınız COM portunu burada belirtin
baud_rate = 38400

# Seri porta bağlanma
ser = serial.Serial(serial_port, baud_rate)
print(f"Bağlantı {serial_port} üzerinden kuruldu.")

x = 0

try:
    while True:        
        y1 = random.randint(0, 100)
        y2 = random.randint(0, 100)
        data = f"{x},{y1},{y2}\n"
        ser.write(data.encode())

        x += 0.1;
        time.sleep(0.1)  # 1 saniye bekleme süresi
        
        print(f"Gönderilen veri: {data.strip()}")

        
        

except KeyboardInterrupt:
    print("Kullanıcı durdurdu.")

finally:
    ser.close()
    print("Bağlantı kapatıldı.")
