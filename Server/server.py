from socket import *

s = socket(AF_INET, SOCK_STREAM)
HOST = '127.0.0.1'
PORT = 6363
s.connect((HOST, PORT))
s.send("update")
while True:
    pass
