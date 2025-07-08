from socket import *

def Test(x:float) -> float:
    return 0

server_port = 50001
server_socket  = socket(AF_INET,SOCK_STREAM)
server_socket.bind(('',server_port))
server_socket.listen(1)
connect_socket,addr = server_socket.accept()
while True:
    # connect_socket,addr = server_socket.accept()
    message, addr = connect_socket.recvfrom(2048)
    print(f"cjj:{message.decode()}")
    reply = input("You:")
    connect_socket.send(reply.encode())
connect_socket.close()
server_socket.close()