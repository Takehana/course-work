#!flask/bin/python
from flask import Flask, jsonify, make_response, request
import sys
import re
import os
import requests as req_forward

# Initialize API fields
app = Flask(__name__)

kv_data = {}

# Implement the in memory key value stores w/ dict.
# TODO: Global variables to determine whether or this instance
# is a client or main node.


# PUT COMMAND
@app.route('/kv-store/<string:key>', methods=['PUT'])
def put_kv(key):
    if not MAIN_NODE:
        try:
            r = req_forward.put('http://' + MAIN_IP + '/kv-store/' + key,
                                data=request.form)
            return make_response(jsonify(r.json())), r.status_code
        except:
            return make_response(
                                 jsonify({'result': 'Error',
                                         'msg': 'Server unavailable'})), 500
# First verifies validity of the key.
key_regex = re.compile('[a-zA-Z0-9_]+$')
if key_regex.match(key) and len(key) >= 1 and len(key) <= 200:
    try:
        value = request.form['val']
        # lol what is this daft code
        except:
            value = ''
            pass
    # Verifies existence of a value.
    if len(value) == 0:
        return make_response(
                             jsonify({'result': 'Error',
                                     'msg': 'No value provided'})), 403
                                     # NOTE: Finds equal sign and takes string to the right.
                                     if sys.getsizeof(value) > 1024:
                                         return make_response(
                                                              jsonify({'result': 'Error',
                                                                      'msg': 'Object too large. Size limit is 1MB'})), \
                                                                  403
                                                                      # The KV pair has now been demonstrated to be valid.
# CASE 1: If existing key value replaced.
if key in kv_data:
    kv_data[key] = value
    return make_response(
                         jsonify({'replaced': 'True',
                                 'msg': 'Value of existing key replaced'})), 200
                             # CASE 2: The key is new.
                             else:
                                 kv_data[key] = value
                                 return make_response(
                                                      jsonify({'replaced': 'False',
                                                              'msg': 'New key created'})), 201
                             # If key is not valid, return 'Key not valid' error.
else:
    return make_response(
                         jsonify({'result': 'Error', 'msg': 'Key not valid'})), 403


# GET COMMAND
@app.route('/kv-store/<string:key>', methods=['GET'])
def get_kv(key):
    if not MAIN_NODE:
        try:
            r = req_forward.get('http://' + MAIN_IP + '/kv-store/' + key)
            return make_response(jsonify(r.json())), r.status_code
        except:
            return make_response(
                                 jsonify({'result': 'Error',
                                         'msg': 'Server unavailable'})), 500
# CASE 1: The key exists.
if key in kv_data:
    return make_response(
                         jsonify({'result': 'Success',
                                 'value': kv_data[key]})), 200
                                 # CASE 2: The key does not exist.
                                 else:
                                     return make_response(
                                                          jsonify({'result': 'Error',
                                                                  'msg': 'Key does not exist'})), 404


# DELETE COMMAND
@app.route('/kv-store/<string:key>', methods=['DELETE'])
def del_kv(key):
    if not MAIN_NODE:
        try:
            r = req_forward.delete('http://' + MAIN_IP + '/kv-store/' + key)
            return make_response(jsonify(r.json())), r.status_code
        except:
            return make_response(
                                 jsonify({'result': 'Error',
                                         'msg': 'Server unavailable'})), 500
# CASE 1: The key exists.
if key in kv_data:
    del kv_data[key]
    return make_response(jsonify({'result': 'Success'})), 200
    # CASE 2: The key does not exist.
    else:
        return make_response(
                             jsonify({'result': 'Error',
                                     'msg': 'Key does not exist'})), 404


if __name__ == '__main__':
    # If MAINIP is not an environment variable in docker container,
    # the node is the main node.
    if os.getenv('MAINIP') is not None:
        MAIN_NODE = False
        MAIN_IP = os.getenv('MAINIP')
    else:
        MAIN_NODE = True
    # Sets the externally visible IP to which this instance binds to.
    EXTERNAL_IP = os.getenv('IP')
    # Sets the port to which your instance binds to.
    PORT = os.getenv('PORT')
    app.run(host=EXTERNAL_IP, port=int(PORT), debug=True)

