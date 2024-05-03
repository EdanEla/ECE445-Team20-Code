import serial
import matplotlib.pyplot as plt
import numpy as np


def reject_outliers(data, m=2):
    return data[abs(data - np.mean(data)) < m * np.std(data)]


adjX = 0
adjY = 0
adjZ = 0


ser = serial.Serial("COM7",115200,timeout=1)

x = np.array([])
y = np.array([])
z = np.array([])
for i in range(1000):
    if ser.isOpen():
        input_data=ser.readline().strip().decode("utf-8").split(",")
        x = np.append(x, [float(input_data[0]) + adjX])
        y = np.append(y, [float(input_data[1]) + adjY])
        z = np.append(z, [float(input_data[2]) + adjZ])
plt.scatter(x, y, color='red')
plt.scatter(y, z, color='blue')
plt.scatter(z, x, color='green')

x = reject_outliers(x, m = 2)
y = reject_outliers(y, m = 2)
z = reject_outliers(z, m = 2)
print((max(x) + min(x))/2)
print((max(y) + min(y))/2)
print((max(z) + min(z))/2)
plt.show()
    
                        
