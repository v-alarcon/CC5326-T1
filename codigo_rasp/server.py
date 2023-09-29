import datetime
import socket
import time
from peewee import PostgresqlDatabase
from modelos import *
import struct

HOST = '0.0.0.0'  # Escucha en todas las interfaces disponibles
PORT = 1234       # Puerto en el que se escucha

# Configuración de la base de datos
db_config = {
    'host': 'localhost', 
    'port': 5432, 
    'user': 'postgres', 
    'password': 'postgres', 
    'database': 'db'
}
db = PostgresqlDatabase(**db_config)

# Crea un socket para IPv4 y conexión TCP
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()

    print("El servidor está esperando conexiones en el puerto", PORT)

    while True:
        conn, addr = s.accept()  # Espera una conexión
        with conn:
            print('Conectado por', addr)
            data = conn.recv(1024)  # Recibe hasta 1024 bytes del cliente
            if data:
                print("Recibido: ", data.decode('utf-8'))
                if data.decode('utf-8') == "protocol_and_transport_layer":
                    # ask to the table Config of database db using peewee by id
                    query = Config.select().where(Config.id == 1)
                    ans=db.execute(query)
                    for row in ans:
                        conn.sendall((str(row[1])+str(row[2])).encode('utf-8')+int(time.time()).to_bytes(4, byteorder='little'))
                        break
                    if str(row[1])+str(row[2])== "00":
                        # DO PROTOCOL 0 VIA TCP
                        # prepare to receive data
                        print("\nPrepare to receive data from Protocol 0")
                        s.listen()
                        conn, addr = s.accept()
                        print('Conectado por', addr)
                        data = conn.recv(1024)
                        print("Recibido los bytes")

                        # send OK to the client
                        conn.sendall("OK".encode('utf-8'))

                        # save data in the database
                        print("Guardando en la base de datos")

                        #get the first 2 bytes from the data and transform into a str id_device
                        id_device = str(data[0:2].decode('utf-8'))
                        print("ID: " + id_device)

                        #get the next 6 bytes from the data and transform into a str MAC
                        mac = ""
                        for i in range(4):
                            mac += bytes.hex(data[2+i:3+i]) + ":"
                        mac = mac[:-1]
                        print("MAC: " + mac)

                        #get the next byte to know the transport layer
                        transport_layer = chr(data[8])
                        print("Transport layer: " + transport_layer)

                        #get the next byte to get the protocol
                        protocol = chr(data[9])
                        print("Protocol: " + protocol)

                        #get the next 2 bytes to get the packet length
                        packet_length = int.from_bytes(data[10:12], signed=False, byteorder='little')
                        print("Packet length: " + str(packet_length))
                        #get the final byte with the battery level
                        battery_level = data[12]
                        print("Battery level: " + str(battery_level))

                        #save the data in the database
                        #Config.insert(ID_protocol='0', Transport_Layer='0').execute()
                        Datos.insert(Id_device=id_device, MAC=mac, battlevel=battery_level).execute()
                        Logs.insert(ID_device=id_device, Transport_Layer=transport_layer, finaltime=datetime.datetime.now()).execute()

   
                    if str(row[1])+str(row[2])== "10":
                        # DO PROTOCOL 1 VIA TCP
                        # prepare to receive data
                        print("\nPrepare to receive data from Protocol 1")
                        s.listen()
                        conn, addr = s.accept()
                        print('Conectado por', addr)
                        data = conn.recv(1024)
                        print("Recibido los bytes")

                        # send OK to the client
                        conn.sendall("OK".encode('utf-8'))

                        # save data in the database
                        print("Guardando en la base de datos")

                        #get the first 2 bytes from the data and transform into a str id_device
                        id_device = str(data[0:2].decode('utf-8'))
                        print("ID: " + id_device)

                        #get the next 6 bytes from the data and transform into a str MAC
                        mac = ""
                        for i in range(4):
                            mac += bytes.hex(data[2+i:3+i]) + ":"
                        mac = mac[:-1]
                        print("MAC: " + mac)

                        #get the next byte to know the transport layer
                        transport_layer = chr(data[8])
                        print("Transport layer: " + transport_layer)

                        #get the next byte to get the protocol
                        protocol = chr(data[9])
                        print("Protocol: " + protocol)

                        #get the next 2 bytes to get the packet length
                        packet_length = int.from_bytes(data[10:12], signed=False, byteorder='little')
                        print("Packet length: " + str(packet_length))

                        #get the final byte with the battery level
                        battery_level = data[12]
                        print("Battery level: " + str(battery_level))

                        #get the timestamp
                        timestamp0 = int.from_bytes(data[13:17], signed=False, byteorder='little')
                        ts = datetime.datetime.fromtimestamp(timestamp0).strftime('%Y-%m-%d %H:%M:%S')
                        print("Timestamp: " + ts)

                        #save the data in the database
                        Datos.insert(Id_device=id_device, MAC=mac, battlevel=battery_level, timestamp=ts).execute()
                        Logs.insert(ID_device=id_device, Transport_Layer=transport_layer, finaltime=datetime.datetime.now(),
                                     initialtime=ts).execute()

                    if str(row[1])+str(row[2])== "20":
                        # DO PROTOCOL 2 VIA TCP
                        # DO PROTOCOL 0 VIA TCP
                        # prepare to receive data
                        print("\nPrepare to receive data from Protocol 2")
                        s.listen()
                        conn, addr = s.accept()
                        print('Conectado por', addr)
                        data = conn.recv(1024)
                        print("Recibido los bytes")

                        # send OK to the client
                        conn.sendall("OK".encode('utf-8'))

                        # save data in the database
                        print("Guardando en la base de datos")

                        #get the first 2 bytes from the data and transform into a str id_device
                        id_device = str(data[0:2].decode('utf-8'))
                        print("ID: " + id_device)

                        #get the next 6 bytes from the data and transform into a str MAC
                        mac = ""
                        for i in range(4):
                            mac += bytes.hex(data[2+i:3+i]) + ":"
                        mac = mac[:-1]
                        print("MAC: " + mac)

                        #get the next byte to know the transport layer
                        transport_layer = chr(data[8])
                        print("Transport layer: " + transport_layer)

                        #get the next byte to get the protocol
                        protocol = chr(data[9])
                        print("Protocol: " + protocol)

                        #get the next 2 bytes to get the packet length
                        packet_length = int.from_bytes(data[10:12], signed=False, byteorder='little')
                        print("Packet length: " + str(packet_length))
                        #get the final byte with the battery level
                        battery_level = data[12]
                        print("Battery level: " + str(battery_level))

                        #get the timestamp
                        timestamp0 = int.from_bytes(data[13:17], signed=False, byteorder='little')
                        ts = datetime.datetime.fromtimestamp(timestamp0).strftime('%Y-%m-%d %H:%M:%S')
                        print("Timestamp: " + ts)

                        #get the temperature from the next byte
                        temperature = data[17]
                        print("Temperature: " + str(temperature))

                        #get the pressure from the next 4 bytes
                        pressure = int.from_bytes(data[18:22], signed=False, byteorder='little')
                        print("Pressure: " + str(pressure))

                        #get the humidity from the next byte its an int
                        humidity = data[22]
                        print("Humidity: " + str(humidity))

                        #get the CO2 from the next 4 bytes, its a float
                        co = struct.unpack('f', data[23:27])
                        print("CO: " + str(co[0]))

                        #save the data in the database
                        Datos.insert(Id_device=id_device, MAC=mac, battlevel=battery_level, timestamp=ts, temp=temperature,
                                     press=pressure, hum=humidity, co=co[0]).execute()
                        Logs.insert(ID_device=id_device, Transport_Layer=transport_layer, finaltime=datetime.datetime.now(),
                                        initialtime=ts).execute()
                        


                    if str(row[1])+str(row[2])== "30":
                        # DO PROTOCOL 3 VIA TCP
                        print("30")
                    if str(row[1])+str(row[2])== "40":
                        # DO PROTOCOL 4 VIA TCP
                        print("40") 
