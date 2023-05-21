import sys
sys.path.insert(1, '../')
import threading
import json
from portal_client import PortalClient
from producer import PortalProducer
import time
import random
config = json.load(open('config.json'))['brokers']
import signal

sensor = None

class SENSOR(threading.Thread):
    def __init__(self, name, id,sleep_time=5):
        threading.Thread.__init__(self)
        self.name = name
        self.id = id
        self.topic = name+'_'+id
        print('==============================================================')
        print('Sensor topic: ', self.topic)
        print('==============================================================')
        self.sleep_time = sleep_time
        self.set_producer()
        self._stopevent = threading.Event()
 
    def set_producer(self):
        self.client = PortalClient(config)
        self.producer = PortalProducer(self.client)

    def get_data(self):
        pass

    def logic(self):
        self.emit()

    def emit(self):
        self.producer.send_message(self.topic, self.get_data())

    def timeout(self):
        time.sleep(self.sleep_time)


    def run(self):
        try:
            while not self._stopevent.isSet():
                self.logic()
                self.timeout()
        except Exception as e:
            print(e)

    def stop(self):
        self._stopevent.set()


class TEMP(SENSOR):
    def __init__(self, name, id,sleep_time=5):
        super().__init__(name, id,sleep_time)
        self.count =0

    def get_data(self):
        data = self.count
        self.count+=1
        print('==============================================================')
        print('TEMP: ', data)
        print('==============================================================')
        return self.id +" :: "+str(data)

def handler(signum, frame):
    print('Shutting down sensor...')
    sensor.stop()


signal.signal(signal.SIGINT, handler)

if __name__ == '__main__':
    sensor_name = sys.argv[1]
    sensor_id = sys.argv[2]
    sensor_timeout = float(sys.argv[3])
    # sensor_name = 'temp'
    # sensor_id = '1'
    # sensor_timeout = 5
    if sensor_name == 'temp':
        sensor = TEMP('temp',sensor_id,sensor_timeout)
        sensor.start()
    else:
        print('Invalid sensor name')