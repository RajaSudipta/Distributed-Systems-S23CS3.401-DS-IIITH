import sys
sys.path.insert(1, '../')
import threading
import json
from portal_client import PortalClient
from consumer import PortalConsumer
import time
import random
config = json.load(open('config.json'))['brokers']
import signal

controller = None

class CONTROLLER(threading.Thread):
    def __init__(self, topic, sleep_time=0):
        threading.Thread.__init__(self)
        self.topic = topic
        print('==============================================================')
        print('Controller topic: ', self.topic)
        print('==============================================================')
        self.sleep_time = sleep_time
        self.set_consumer()
        self._stopevent = threading.Event()

    def set_consumer(self):
        self.client = PortalClient(config)
        self.consumer = PortalConsumer(self.client,self.topic)

    def do_action(self, message):
        print(message)

    def run(self):
        while True:
            message = self.consumer.get_message()
            if self._stopevent.isSet():
                break
            self.do_action(message)

    def stop(self):
        self._stopevent.set()
        self.consumer.stop()


def handler(signum, frame):
    print('Shutting down controller...')
    controller.stop()


signal.signal(signal.SIGINT, handler)

if __name__ == '__main__':
    controller_topic = sys.argv[1]
    # controller_topic = 'temp_1'
    controller = CONTROLLER(controller_topic)
    controller.start()
