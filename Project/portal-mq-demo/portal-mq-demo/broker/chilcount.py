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
children = zk.get_children('/logs')

print(len(children))