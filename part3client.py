import socket

SERVER_IP = "192.168.142.156"  
PORT = 8080  

def download_file(filename):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((SERVER_IP, PORT))

    client_socket.sendall(filename.encode())  # Send filename to server
    response = client_socket.recv(1).decode()  # Read the first byte to check response type

    if response == 'D':  # File exists, start downloading
        with open(f"downloaded_{filename}", 'wb') as f:
            while True:
                data = client_socket.recv(100)  # Receive 100-byte chunks
                if not data:
                    break
                f.write(data)
        print(f"File '{filename}' downloaded successfully.")
    elif response == 'E':  # Error message received
        error_msg = client_socket.recv(1024).decode()
        print(f"Error: {error_msg}")

    client_socket.close()

if __name__ == "__main__":
    filename = input("Enter the filename to download: ")
    download_file(filename)
