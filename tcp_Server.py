import socket
import threading
import time
import random
import string

HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 8080        # TCP Port

def generate_large_data():
    """Generate a data packet larger than 500 bytes"""
    timestamp = f"Data packet at {time.strftime('%Y-%m-%d %H:%M:%S')}\n"
    padding = ''.join(random.choices(string.ascii_letters + string.digits, k=29))  # Generate 1000 bytes of random data
    return (timestamp + padding)[:1024]  # Ensure the total length is at least 1024 bytes


def handle_receive(conn, addr):
    print("Starting Receive Thread")
    """Function to receive data from the client"""
    try:
        while True:
            client_data = conn.recv(1024).decode().strip()
            if not client_data:
                print(f"[!] Client {addr} disconnected")
                break
            print(f"[Client {addr}] Received: {client_data}")
    except (BrokenPipeError, ConnectionResetError):
        print(f"[!] Connection lost with {addr}")
    finally:
        conn.close()

def handle_send(conn, addr):
    """Function to send data to the client every second"""
    print("Starting send Thread")
    try:
        while True:
            #server_data = f"Data packet at {time.strftime('%Y-%m-%d %H:%M:%S')}\n"
            server_data = generate_large_data()
            conn.sendall(server_data.encode())
            print("[Server]",server_data)
            time.sleep(0.25)
    except (BrokenPipeError, ConnectionResetError):
        print(f"[!] Connection lost with {addr}")
    finally:
        conn.close()

def start_server():
    """Main server function"""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((HOST, PORT))
        server_socket.listen(5)  # Accept multiple clients

        print(f"[*] TCP Server listening on {HOST}:{PORT}")

        while True:
            conn, addr = server_socket.accept()  # Accept a new client
            print(f"[+] Connection established with {addr}")

            # Start separate threads for sending and receiving
            recv_thread = threading.Thread(target=handle_receive, args=(conn, addr), daemon=True)
            send_thread = threading.Thread(target=handle_send, args=(conn, addr), daemon=True)

            recv_thread.start()
            send_thread.start()

if __name__ == "__main__":
    start_server()
