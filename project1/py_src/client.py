from socket import *

server_name = "10.249.43.100"
server_port = 50001

client_socket = socket(AF_INET,SOCK_DGRAM)
print("Talking with boss")
while True:
    message = input("You:")
    client_socket.sendto(message.encode(),(server_name,server_port))
    reply,server_address = client_socket.recvfrom(2048)
    print(f"boss:{reply.decode()}")
client_socket.close()