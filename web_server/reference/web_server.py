#import socket module
from socket import *
import sys # In order to terminate the program

serverSocket = socket(AF_INET, SOCK_STREAM)

# Prepare a sever socket
# Fill in start
serverPort = 6789  # You can change this port if needed
serverSocket.bind(('', serverPort))  # Bind the server socket to all interfaces and port
serverSocket.listen(1)  # Listen for incoming connections (max 1 connection at a time)
# Fill in end

while True:
    # Establish the connection
    print('Ready to serve...')
    connectionSocket, addr = serverSocket.accept()  # Fill in start -> accept incoming connection
    # Fill in end
    
    try:
        breakpoint()
        message = connectionSocket.recv(1024).decode()  # Fill in start -> receive HTTP request
        # Fill in end
        
        filename = message.split()[1]
        f = open(filename[1:])  # Remove leading '/'
        outputdata = f.read()  # Fill in start -> read requested file
        # Fill in end

        # Send one HTTP header line into socket
        # Fill in start
        response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
        connectionSocket.send(response_header.encode())
        # Fill in end

        # Send the content of the requested file to the client
        for i in range(0, len(outputdata)):
            connectionSocket.send(outputdata[i].encode())
        connectionSocket.send("\r\n".encode())

        connectionSocket.close()

    except IOError:
        # Send response message for file not found
        # Fill in start
        response_404 = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>"
        connectionSocket.send(response_404.encode())
        # Fill in end

        # Close client socket
        # Fill in start
        connectionSocket.close()
        # Fill in end

serverSocket.close()
sys.exit()  # Terminate the program after sending the corresponding data