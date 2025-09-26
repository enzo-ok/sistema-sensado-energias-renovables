import serial
import struct

ser = serial.Serial('/dev/ttyS0', 115200)

while 1:
	data= ser.read(8)
	ser.flushInput()
	valores = []
	for i in range(0, 8, 2):
		valor = int.from_bytes(data[i:i+2], byteorder='little', signed=False)
		valores.append(valor)
	
	conversion = (valores[2]*(3.3/1023))*4 #convierto al valor real el dato leido por el ADC en el nodo X.
	valBat = round(conversion, 2)  #dejo solo 2 decimlaes para el valor de tension de bateria
	VelWind = ((valores[3]*2*3.14159265*6)/(5*1000))
	conversion2 = round(VelWind,2)
	
	if valores[1] == 0x00:
		cardinal_point = "NE"
	elif valores[1] == 0x01:
		cardinal_point = "NNE"
	elif valores[1] == 0x02:
		cardinal_point = "N"
	elif valores[1] == 0x03:
		cardinal_point = "NNO"
	elif valores[1] == 0x04:
		cardinal_point = "NO"
	elif valores[1] == 0x05:
		cardinal_point = "ONO"
	elif valores[1] == 0x06:
		cardinal_point = "O"
	elif valores[1] == 0x07:
		cardinal_point = "OSO"
	elif valores[1] == 0x08:
		cardinal_point = "SO"
	elif valores[1] == 0x09:
		cardinal_point = "SSO"
	elif valores[1] == 0x0A:
		cardinal_point = "S"
	elif valores[1] == 0x0B:
		cardinal_point = "SSE"
	elif valores[1] == 0x0C:
		cardinal_point = "SE"
	elif valores[1] == 0x0D:
		cardinal_point = "ESE"
	elif valores[1] == 0x0E:
		cardinal_point = "E"
	elif valores[1] == 0x0F:
		cardinal_point = "ENE"
	else:
		cardinal_point = valores[1] 
	
	print("\nEsclavo: ",valores[0])
	print("Punto cardinal: ",cardinal_point)
	print("Tension de Bateria: ",valBat,"V")
	print("Viento: ",conversion2,"km/h")
	
