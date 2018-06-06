from socket import socket, AF_INET, SOCK_STREAM


def get_dll():
    with open("TcpServer.dll", "rb") as m_file:
        return m_file.read()


def to_bytes(n, length, endianess='big'):
    h = '%x' % n
    s = ('0' * (len(h) % 2) + h).zfill(length * 2).decode('hex')
    return s if endianess == 'big' else s[::-1]


def main():
    s = socket(AF_INET, SOCK_STREAM)

    HOST = '127.0.0.1'
    PORT = 6363

    try:
        s.connect((HOST, PORT))

        value = ""

        while value != "quit":

            value = raw_input("> ")
            value = value.replace('\r', '')

            if value == "update":
                dll = get_dll()
                bytes_value = to_bytes(len(dll) + 5, 4, 'little')
                s.send('u' + bytes_value + dll)

            else:
                s.send(value)

        s.send("")
    except:
        print("Could not open socket on port %d" % PORT)


if __name__ == '__main__':
    main()
