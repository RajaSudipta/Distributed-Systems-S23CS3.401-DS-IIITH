from flask import Flask, request, jsonify
from kazoo.client import KazooClient
from kazoo.exceptions import NoNodeError
from kazoo.recipe import election
from threading import Thread
from kazoo.recipe.watchers import ChildrenWatch
from kazoo.recipe.lock import Lock

# PORT = sys.argv[1]
zkhost='localhost:2181'
zk = KazooClient(hosts=zkhost)
zk.start()
zk.delete('/election',recursive=True)
zk.delete('/leader',recursive=True)
zk.delete('/message_queue',recursive=True)
zk.delete('/logs',recursive=True)
zk.delete('/consumers',recursive=True)
zk.delete('/locks',recursive=True)
print('Cleared Zookeeper')