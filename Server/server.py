from socket import socket, AF_INET, SOCK_STREAM

s = socket(AF_INET, SOCK_STREAM)
HOST = '127.0.0.1'
PORT = 6363
# try:
s.connect((HOST, PORT))

value = ""

while value != "quit":
    value = raw_input("> ")
    value = value.replace('\r', '')
    s.send(value)

s.send()
# except:
#     print("Could not open socket on port %d" % PORT)
