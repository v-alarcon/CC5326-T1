## Plantilla T1

### Integrantes

- Nombre y Apellido
- Nombre y Apellido
- Nombre y Apellido

---

### Iniciar la base de datos
Para iniciar la base de datos en docker
```bash
docker compose up -d
```
Luego para crear las tablas se debe ejecutar codigo_rasp\modelos.py, si se necesita la configuracion inicial en la tabla config descomentar la ultima linea.

## Iniciar servidor
Se debe iniciar el servidor ejecutando el programa codigo_rasp\server.py

## Flashear ESP32
Primero se debe modificar las credenciales de WiFi que estan en la linea 21 de codigo_esp\main\test.c
Se flashea la esp32 usando esp-idf con el proyecto dentro de codigo_esp y segun la tabla config se va a ejecutar el protocolo correspondiente