from portal_client import PortalClient
class PortalProducer:
    def __init__(self,client : PortalClient):
        self.client = client
        self.register()

    def register(self):
        resp,_=self.client.request('POST','/producer/create',data={})
        # print(resp)

    def unregister(self):
        resp,_=self.client.request('POST','/producer/delete',data={})
        # print(resp)

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

    def send_message(self,topic,message:str):
        if not self.is_exists_topic(topic):
            self.create_topic(topic)
        data = {
            "name":topic,
            "message":message
        }
        while True:
            resp,status=self.client.request('POST','/publish',data)
            if status == 200:
                break
        # print(resp)

    def __del__(self):
        self.unregister()
