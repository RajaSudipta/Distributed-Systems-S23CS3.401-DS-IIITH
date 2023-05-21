from flask import Flask, request, jsonify
from kazoo.client import KazooClient
from kazoo.exceptions import NoNodeError
from kazoo.recipe import election
import uuid
import sys
import uuid
import sqlite3
import json

app = Flask(__name__)
id = str(uuid.uuid4())
HOST = '127.0.0.1'
#PORT = 5000
PORT = sys.argv[1]
zk = KazooClient(hosts='localhost:2181')
zk.start()

zk.ensure_path('/election')
zk.ensure_path('/leader')
zk.ensure_path('/message_queue')
election_node = zk.create('/election/node-'+id, ephemeral=True)

# con = sqlite3.connect(str(PORT) + ".sqlite")


# Define a function to connect to the database
def get_db_connection():
    conn = sqlite3.connect(str(PORT) + ".sqlite")
    conn.row_factory = sqlite3.Row
    return conn


# Define a function to close the database connection
def close_db_connection(conn):
    conn.close()


def db_init():
    con = get_db_connection()
    cur = con.cursor()

    drop_consumer_table = "DROP TABLE IF EXISTS consumer;"
    drop_producer_table = "DROP TABLE IF EXISTS producer;"
    drop_topic_table = "DROP TABLE IF EXISTS topic;"
    drop_message_queue_table = "DROP TABLE IF EXISTS message_queue;"

    cur.execute(drop_consumer_table)
    cur.execute(drop_producer_table)
    cur.execute(drop_topic_table)
    cur.execute(drop_message_queue_table)

    create_consumer_table = "CREATE TABLE consumer (id varchar(100) NOT NULL, PRIMARY KEY (id));"
    create_producer_table = "CREATE TABLE producer (id varchar(100) NOT NULL, PRIMARY KEY (id));"
    create_topic_table = "CREATE TABLE topic (id INTEGER PRIMARY KEY AUTOINCREMENT, name varchar(100) NOT NULL UNIQUE, offset INTEGER NOT NULL, size INTEGER NOT NULL);"
    create_message_queue_table = "CREATE TABLE message_queue (id INTEGER NOT NULL, seq_no INTEGER NOT NULL, message varchar(500), PRIMARY KEY (id, seq_no), FOREIGN KEY(id) REFERENCES topic(id));"

    cur.execute(create_consumer_table)
    cur.execute(create_producer_table)
    cur.execute(create_topic_table)
    cur.execute(create_message_queue_table)

    con.commit()
    close_db_connection(con)


def become_leader():
    leader_loc = {'host': HOST, 'port': PORT}
    leader_loc_str = json.dumps(leader_loc)
    zk.set('/leader', leader_loc_str.encode())
    print("<Leader>")


def start_election():
    # Create an ephemeral node with a unique sequential name
    # Get the list of all ephemeral nodes under /election
    election_nodes = zk.get_children('/election')
    # Sort the nodes in ascending order
    election_nodes.sort()
    # If this node has the lowest sequence number, it becomes the leader
    if election_node == f'/election/{election_nodes[0]}':
        become_leader()


@zk.ChildrenWatch("/election")
def election_watcher(event):
    # If a node is deleted, start a new election
    start_election()


@app.route('/')
def health():
    return 'Healthy'


@app.route('/publish', methods=['POST'])
def publish_message():
    # # Get the message from the request body
    # message = request.json.get('message')
    # # Create a new znode with the message as the data
    # zk.create('/message_queue/message_', value=message.encode(), sequence=True)
    # return 'Message published successfully.'

    name = request.json.get('name')
    message = request.json.get('message')

    command = "SELECT * FROM topic WHERE name = '" + str(name) + "';"

    con = get_db_connection()

    # Reading id, size from topic table
    try:
        cur = con.cursor()
        cur.execute(command)
        records = cur.fetchall()
        con.commit()
        close_db_connection(con)

        id = records[0][0]
        size = records[0][3]
        new_size = size+1
        seq_no = size+1

        # Update the size of the existing queue in topic table
        try:
            update_command = "UPDATE topic SET size = " + str(new_size) + " WHERE name = '" + str(name) + "';"
            con = get_db_connection()
            cur = con.cursor()
            cur.execute(update_command)
            records = cur.fetchall()
            con.commit()
            close_db_connection(con)

            # Writing id, seq_no, message in message_queue
            try:
                write_command = "INSERT INTO message_queue VALUES(" + str(id) + ", " + str(seq_no) + ", '" + str(message) + "');"
                con = get_db_connection()
                cur = con.cursor()
                cur.execute(write_command)
                records = cur.fetchall()
                con.commit()
                close_db_connection(con)
                return jsonify({'message': 'published in topic successfully'})
            except:
                con.abort()
                close_db_connection(con)
                return jsonify({'message': 'writing in message_queue table unsuccessful!!'}), 501
        except:
            con.abort()
            close_db_connection(con)
            return jsonify({'message': 'UPDATE topic table unsuccessful!!'}), 501
    except:
        con.abort()
        close_db_connection(con)
        return jsonify({'message': 'reading from topic table unsuccessful!!'}), 501


@app.route('/consume', methods=['POST'])
def consume_message():
    # # Get the current leader of the election
    # # Get a list of all messages in the queue
    # messages = zk.get_children('/message_queue')
    # sorted_messages = sorted(messages)
    # # Get the first message in the queue
    # if len(sorted_messages) > 0:
    #     message_path = '/message_queue/' + sorted_messages[0]
    #     message, _ = zk.get(message_path)
    #     message = message.decode()

    #     # Delete the message from the queue
    #     zk.delete(message_path)

    #     return message
    # else:
    #     return 'No messages in the queue.'

    name = request.json.get('name')
    command = "SELECT * FROM topic WHERE name = '" + str(name) + "';"

    con = get_db_connection()

    # Reading id, size from topic table
    try:
        cur = con.cursor()
        cur.execute(command)
        records = cur.fetchall()
        con.commit()
        close_db_connection(con)

        id = records[0][0]
        offset = records[0][2]
        size = records[0][3]

        if(offset < size):
            fetch_command = "SELECT * FROM message_queue WHERE id = " + str(id) + " AND seq_no = " + str(offset+1) + ";"
            con = get_db_connection()
            # Fetch from message_queue table
            try:
                cur = con.cursor()
                cur.execute(fetch_command)
                records = cur.fetchall()
                con.commit()
                close_db_connection(con)
                message = records[0][2]
                try:
                    # Update topic table, increase offset
                    update_command = "UPDATE topic SET offset = " + str(offset+1) + " WHERE name = '" + str(name) + "';"
                    con = get_db_connection()
                    cur = con.cursor()
                    cur.execute(update_command)
                    records = cur.fetchall()
                    con.commit()
                    close_db_connection(con)
                    return jsonify({'message' : message})
                except:
                    con.abort()
                    close_db_connection(con)
                    return jsonify({'message': 'Update topic table offset unsuccessful!!'}), 501
            except:
                con.abort()
                close_db_connection(con)
                return jsonify({'message': 'Fetch from message_queue table unsuccessful!!'}), 501
        else:
            return jsonify({'message': ''}), 204
    except:
        con.abort()
        close_db_connection(con)
        return jsonify({'message': 'Reading from topic table unsuccessful!!'}), 501


# Watch for changes in the leadership
# @zk.DataWatch('/election')
# def watch_leader(data, stat):
#     global election_obj

#     # If the leader is no longer the current client, update the election object
#     if len(election_obj.contenders())>0 and election_obj.contenders()[0] != zk.client_id[1]:
#         election_obj = election.Election(zk, '/election')


@zk.ChildrenWatch("/message_queue")
def watch_children(children):
    print("Children are now: %s" % children)

# @zk.DataWatch("/my/favorite")
# def watch_node(data, stat):
#     print("Version: %s, data: %s" % (stat.version, data.decode("utf-8")))


@app.route("/consumer/create", methods=['POST'])
def create_consumer():
    # Get the message from the request body
    id = request.json.get('id')

    command = "INSERT INTO consumer VALUES(" + str(id) + ");"

    con = get_db_connection()
    try:
        cur = con.cursor()
        cur.execute(command)
        con.commit()
        close_db_connection(con)
        return jsonify({'message': 'consumer created successfully'})
    except:
        con.abort()
        close_db_connection(con)
        return jsonify({'message': 'consumer created unsuccessful!!'}), 501


@app.route("/consumer/delete", methods=['POST'])
def delete_consumer():
    # Get the message from the request body
    id = request.json.get('id')

    # DELETE FROM artists_backup WHERE artistid = 1;
    
    command = "DELETE FROM consumer WHERE id = " + str(id) + ";"

    con = get_db_connection()
    try:
        cur = con.cursor()
        cur.execute(command)
        con.commit()
        close_db_connection(con)
        return jsonify({'message': 'consumer deleted successfully'})
    except:
        con.abort()
        close_db_connection(con)
        return jsonify({'message': 'consumer deleted unsuccessful!!'}), 501


@app.route("/producer/create", methods=['POST'])
def create_producer():
    # Get the message from the request body
    id = request.json.get('id')
    
    command = "INSERT INTO producer VALUES(" + str(id) + ");"

    con = get_db_connection()
    try:
        cur = con.cursor()
        cur.execute(command)
        con.commit()
        close_db_connection(con)
        return jsonify({'message': 'producer created successfully'})
    except:
        con.abort()
        close_db_connection(con)
        return jsonify({'message': 'producer creation unsuccessful!!'}), 501


@app.route("/producer/delete", methods=['POST'])
def delete_producer():
    # Get the message from the request body
    id = request.json.get('id')

    # DELETE FROM artists_backup WHERE artistid = 1;
    
    command = "DELETE FROM producer WHERE id = " + str(id) + ";"

    con = get_db_connection()
    try:
        cur = con.cursor()
        cur.execute(command)
        con.commit()
        close_db_connection(con)
        return jsonify({'message': 'producer deleted successfully'})
    except:
        con.abort()
        close_db_connection(con)
        return jsonify({'message': 'producer deletion unsuccessful!!'}), 501


@app.route("/topic/exists", methods=['POST'])
def exists_topic():
    # Get the message from the request body
    name = request.json.get('name')

    command = "SELECT * FROM topic WHERE name = '" + str(name) + "';"

    print(command)

    con = get_db_connection()
    try:
        cur = con.cursor()
        cur.execute(command)
        rows = cur.fetchall()

        print("len: " + str(len(rows)))

        con.commit()
        close_db_connection(con)

        if(len(rows) > 0):
            return jsonify({'message': True})
        else:
            return jsonify({'message': False})
    except:
        con.abort()
        close_db_connection(con)
        return jsonify({'message': 'checking unsuccessful!!'}), 501


@app.route("/topic/create", methods=['POST'])
def create_topic():
    # Get the message from the request body
    name = request.json.get('name')
    # offset = request.json.get('offset')

    command = "INSERT INTO topic (name, offset, size) VALUES('" + str(name) + "', 0, 0)" + ";"

    print(command)

    con = get_db_connection()
    try:
        cur = con.cursor()
        cur.execute(command)
        con.commit()
        close_db_connection(con)
        return jsonify({'message': 'topic created successfully'})
    except:
        con.abort()
        close_db_connection(con)
        return jsonify({'message': 'topic creation unsuccessful!!'}), 501


@app.route("/topic/delete", methods=['POST'])
def delete_topic():
    # Get the message from the request body
    name = request.json.get('name')

    # DELETE FROM artists_backup WHERE artistid = 1;
    
    command = "DELETE FROM topic WHERE name = '" + str(name) + "';"

    con = get_db_connection()
    try:
        cur = con.cursor()
        cur.execute(command)
        con.commit()
        close_db_connection(con)
        return jsonify({'message': 'topic deleted successfully'})
    except:
        con.abort()
        close_db_connection(con)
        return jsonify({'message': 'topic deletion unsuccessful!!'}), 501


if __name__ == '__main__':
    start_election()
    db_init()
    app.run(host="0.0.0.0", port=PORT, debug=True, use_debugger=False,
            use_reloader=False, passthrough_errors=True)
