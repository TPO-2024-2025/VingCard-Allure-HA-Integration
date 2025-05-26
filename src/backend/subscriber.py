import paho.mqtt.client as mqtt
import sqlite3

# IP address of the device hosting the MQTT broker
# Example: 192.168.43.212
BROKER_IP = "192.168.43.212"

TOPIC = "#"

class Database(object):
    _instance = None
    _connection_obj = None
    _cursor_obj = None
    def __new__(self):
        if self._instance is None:
            print('Creating the object')
            self._instance = super(Database, self).__new__(self)

            # Put any initialization here.
            self.connection_obj = sqlite3.connect('../../ha-vingcard-dev/config/rooms.db')
            self.cursor_obj = self.connection_obj.cursor()

        return self._instance
    
    def updateRoom(self, params):

        self.cursor_obj.execute(
            """UPDATE ROOMS SET state = ?
            WHERE room_num = ?;""",
            params
        )
        self.connection_obj.commit()
    
    def createRoomsTable(self):
        #uncomment if you change the table itself
        #cursor_obj.execute("DROP TABLE IF EXISTS ROOMS")

        self.cursor_obj.execute(
            """CREATE TABLE IF NOT EXISTS ROOMS (
            room_num INT NOT NULL,
            state VARCHAR(25) NOT NULL
            );"""
        )
    
    def intializeRooms(self):
        # some initial values uncomment if you're going to delete the table or all of it's values
        # updating the values in on_message won't work if the table is empty
        for i in range(101, 107):
            self.cursor_obj.execute(
                """INSERT INTO ROOMS VALUES(?, 'IDL')""",
                (i,)
            ) 


def on_connect(client, userdata, flags, rc):
    if rc==0:
        print("Connected OK Returned code = " + str(rc))
    else:
        print("Bad connection Returned code = " + str(rc))

def on_message(client, userdata, message):
    payload = message.payload.decode("utf-8")
    room = message.topic
    params = (payload, room)
    print(f"Message recieved '{str(payload)}' on topic {str(room)}")
    
    db = Database()
    db.updateRoom(params)

if __name__ == "__main__":
    db = Database()
    db.createRoomsTable()

    subscriber = mqtt.Client(client_id="subscriber")
    subscriber.on_connect = on_connect
    subscriber.on_message = on_message
    subscriber.username = "headquarters"
    subscriber.password = "1337"
    print("Connecting to broker . . .")
    subscriber.connect(BROKER_IP)
    subscriber.subscribe(TOPIC)
    subscriber.loop_forever()
