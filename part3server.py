import socket
import os

HOST = '0.0.0.0'  # Listen on all available interfaces
PORT = 8080  

def handle_client(conn):
    filename = conn.recv(1024).decode()  # Receive filename request from client
    print(f"Client requested file: {filename}")

    if os.path.exists(filename):  # Check if file exists
        conn.sendall(b'D')  # Send "D" to indicate file exists
        with open(filename, 'rb') as f:
            while chunk := f.read(100):  # Read 100 bytes at a time
                conn.sendall(chunk)  # Send file data to client
        print("File sent successfully.")
    else:
        conn.sendall(b'EFile not found')  # Send error message with "E" prefix

    conn.close()  # Close connection

def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen(5)  # Allow up to 5 connections

    print(f"Server listening on {HOST}:{PORT}")
    while True:
        conn, addr = server_socket.accept()
        print(f"Connected to {addr}")
        handle_client(conn)

if __name__ == "__main__":
    start_server()
