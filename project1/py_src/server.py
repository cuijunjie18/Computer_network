from socket import *

server_port = 50001
server_socket = socket(AF_INET,SOCK_DGRAM)
server_socket.bind(('',server_port))
print("Talking with cjj")
while True:
    message,client_address = server_socket.recvfrom(2048)
    print(f"cjj:{message.decode()}")
    reply = input("You:")
    server_socket.sendto(reply.encode(),client_address)
server_socket.close()