## Setup

### Zookeeper configuration
```
localhost:2181
```

### Broker Setup
```bash
python3 server.py 5000
python3 server.py 5001
python3 server.py 5002
```
### Requirements:
- Zookeeper (kazoo - python)
- REST API (flask - python)
- Threading (python)

### Why Zookeeper is important ?

Creates a reliable communication mechanism among different brokers to coordinate certain actions.

## Demo
Simulation of Sensors and Controllers in an IoT environment where Sensors are producers and Controllers are consumers.

### Sensors
```bash
python3 sensor.py temp 1 5
python3 sensor.py temp 1 2
python3 sensor.py temp 2 3
```

### Controllers
```bash
python3 controller.py temp_1
python3 controller.py temp_1
python3 controller.py temp_2
```

### Atleast Once Delivery
- `Producer -> Broker:` Producer keeps on retrying to send the message incase of network failure until it recieves an acknowledgement from broker.
- `Broker -> Consumer:` Broker first sends the message with its sequence number, only when consumer recieves the message it tells the broker to consume and only then the message is consumed and offset is incremented.

### Consumers Subscribe
- Consumers automatically sleep when there is no message in the queue.
- Whenever a new message arrives all the consumers are notified which are in sleep mode and they compete to consume a message from the queue.

### Broker Replication
- Shutdown leader and check which new leader gets elected and check if all operations have been executed.
- If running producers and consumers automatically aligns themselves to the new leader.