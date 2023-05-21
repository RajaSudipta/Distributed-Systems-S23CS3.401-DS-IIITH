import threading
from portal_client import PortalClient
class PortalConsumer:
    def __init__(self,client : PortalClient,topic):
        
        self.client = client
        self.topic =topic
        self.__register()
        if not self.is_exists_topic(self.topic):
            self.create_topic(self.topic)
        self._event = threading.Event()
        
        @self.client.zk.DataWatch('locks/topics/{}'.format(topic))
        def watch_lock_status(data, stat):
            # This function will be called whenever the data of the "/leader" node changes
            lock_state = data.decode()
            if lock_state == '0':
                self._event.set()
            else:
                self._event.clear()
            # print('Lock State :'+lock_state)

    def __register(self):
        resp,_=self.client.request('POST','/consumer/create',data={})
        self.client.zk.create('/consumers/{}'.format(self.client.id),ephemeral=True)
        # self.client.zk.create('/locks/topics/{}/{}'.format(self.topic,self.client.id), b'0',ephemeral=True)
        # print(resp['message'])

    def __unregister(self):
        resp,_=self.client.request('POST','/consumer/delete',data={})
        # print(resp['message'])
    
    def stop(self):
        self._event.set()
    # def __del__(self):
    #     self.__unregister()

    def is_exists_topic(self,topic):
        data = {
            "name":topic
        }
        resp,_=self.client.request('POST','/topic/exists',data)
        return resp['message']

    def create_topic(self,topic):
        data = {
            "name":topic
        }
        resp,_=self.client.request('POST','/topic/create',data)
        # print(resp['message'])

    def register_topic(self,topic):
        self.client.zk.create('topics/{}/'.format(topic),ephemeral=True)

    def read_message(self,topic):
        try:
            resp,status=self.client.request('POST','/read',data={"name":topic})
            if status == 200:
                return resp
        except Exception:
            pass
        return None
    

    def get_message(self):
        """Blocking call that returns a message from the given topic.

        Args:
            topic (str): Topic of the message queue

        Returns:
            str: Message
        """
        while True:
            resp_mesg = self.read_message(self.topic)
            if resp_mesg is None:
                self._event.wait()
                continue
            _,status=self.client.request('POST','/consume',data={"name":self.topic,"offset":resp_mesg['offset']})
            # self.topic_unlock(topic)
            if status == 200:
                return resp_mesg['message']
    
    def  __del__(self):
        self.__unregister()