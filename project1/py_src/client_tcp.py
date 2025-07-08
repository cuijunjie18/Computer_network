from socket import *

server_name = "localhost"
server_port = 50001

client_socket = socket(AF_INET,SOCK_STREAM) # TCP连接
client_socket.connect((server_name,server_port))
while True:
    message = input("You:")
    client_socket.send(message.encode())
    reply,server_address = client_socket.recvfrom(2048)
    print(f"boss:{reply.decode()}")
client_socket.close()