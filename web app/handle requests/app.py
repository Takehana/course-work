import os
from flask import Flask, request
app = Flask(__name__)

@app.route('/')
def instr():
    return '"/hello" to see greetings.<br>"/echo?=(message body)" to echo message.'

@app.route('/hello', methods=['GET'])
def hello_world():
    return 'Hello world!'

@app.route('/echo', methods=['GET'])
def echo():
    message = request.args.get('msg', default = '')
    return message

@app.route('/echo', methods=['GET'])
def echo_nothing():
    return ''

if __name__ == '__main__':
    app.debug = True
    host = os.environ.get('IP', '0.0.0.0')
    port = int(os.environ.get('PORT', 8080))
    app.run(port=port)
