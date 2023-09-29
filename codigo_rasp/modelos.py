from peewee import *
from playhouse.postgres_ext import ArrayField
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
    Id_device = CharField()
    MAC = CharField()
    battlevel = IntegerField()
    timestamp = DateTimeField(null = True)

    temp = IntegerField(null = True)
    press = IntegerField(null = True)
    hum = IntegerField(null = True)    
    co = FloatField(null = True)

    ampx = FloatField(null = True)
    freqx = FloatField(null = True)
    ampy = FloatField(null = True)
    freqy = FloatField(null = True)
    ampz = FloatField(null = True)
    freqz = FloatField(null = True)
    rms = FloatField(null = True)


    accx = ArrayField(FloatField, null = True)
    accy = ArrayField(FloatField, null = True)
    accz = ArrayField(FloatField, null = True)
    rgyrx = ArrayField(FloatField, null = True)
    rgyry = ArrayField(FloatField, null = True)
    rgyrz = ArrayField(FloatField, null = True)
    

# create a new table named Logs with ID_device, Transport_Layer and timestamp
class Logs(BaseModel):
    ID_device = CharField()
    Transport_Layer = CharField()
    initialtime = DateTimeField(null = True)
    finaltime = DateTimeField()

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

# the first column is the id of the protocol '0' means Protocolo 0, '1' means Protocolo 1, etc.
# the second one is the transport layer '0' means TCP, '1' means UDP

# insert the initial config row into the table Config
#Config.insert(ID_protocol='0', Transport_Layer='0').execute()