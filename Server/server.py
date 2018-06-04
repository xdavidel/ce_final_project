from socket import socket, AF_INET, SOCK_STREAM


def get_dll():
    with open("TcpServer.dll", "rb") as m_file:
        return m_file.read()


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
                s.send('u' + str(len(dll)))
                s.send(dll)

            else:
                s.send(value)

        s.send("")
    except:
        print("Could not open socket on port %d" % PORT)


if __name__ == '__main__':
    main()
