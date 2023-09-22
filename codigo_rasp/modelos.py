from peewee import Model, PostgresqlDatabase, DateTimeField, CharField

# Configuración de la base de datos
db_config = {
    'host': 'localhost', 
    'port': 5432, 
    'user': 'postgres', 
    'password': 'postgres', 
    'database': 'db'
}
db = PostgresqlDatabase(**db_config)

# Definición de un modelo
class BaseModel(Model):
    class Meta:
        database = db

# Ahora puedes definir tus modelos específicos heredando de BaseModel
# y db estará conectado al servicio de PostgreSQL cuando realices operaciones de base de datos.


## Ver la documentación de peewee para más información, es super parecido a Django

# create a new table named Datos with timestamp, Id_device and MAC
class Datos(BaseModel):
    timestamp = DateTimeField()
    Id_device = CharField()
    MAC = CharField()

# create a new table named Logs with ID_device, Transport_Layer and timestamp
class Logs(BaseModel):
    ID_device = CharField()
    Transport_Layer = CharField()
    timestamp = DateTimeField()

# create a new table named Config with ID_protocol and Transport_Layer
class Config(BaseModel):
    ID_protocol = CharField()
    Transport_Layer = CharField()

# create a new table named Loss with timestap and packet_loss
class Loss(BaseModel):
    timestamp = DateTimeField()
    packet_loss = CharField()

# create the tables in the database
db.create_tables([Datos, Logs, Config, Loss])

# insert the config row into the table Config
# Id_protocol could be Protocolo 0, Protocolo 1, Protocolo 2, Protocolo 3 or Protocolo 4
# Transport_Layer could be TCP or UDP

# insert the initial config row into the table Config
Config.insert(ID_protocol='Protocolo 0', Transport_Layer='TCP').execute()