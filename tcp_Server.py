import socket
import threading
import time
import random
import string

HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 8080        # TCP Port

def generate_large_data(data_length=25):
    """Generate a data packet with a timestamp and configurable random data length."""
    timestamp = f"Data packet at {time.strftime('%Y-%m-%d %H:%M:%S')}\n"
    padding = ''.join(random.choices(string.ascii_letters + string.digits, k=data_length))
    return timestamp + padding

def handle_receive(conn, addr, stop_event):
    print("Starting Receive Thread")
    """Function to receive data from the client"""
    try:
        while not stop_event.is_set():
            client_data = conn.recv(1024).decode().strip()
            if not client_data:
                print(f"[!] Client {addr} disconnected")
                stop_event.set()  # Signal the sender thread to stop
                break
            print(f"[Client {addr}] Received: {client_data}\r\n")
    except (BrokenPipeError, ConnectionResetError):
        print(f"[!] Connection lost with {addr}")
        stop_event.set()  # Ensure sender stops too

def handle_send(conn, addr, stop_event):
    """Function to send data to the client"""
    print("Starting Send Thread")
    try:
        while not stop_event.is_set():
            server_data = generate_large_data()
            conn.sendall(server_data.encode())
            print("[Server]", server_data)
            time.sleep(0.25)
    except (BrokenPipeError, ConnectionResetError):
        print(f"[!] Connection lost with {addr}")
    except OSError as e:
        print(f"[!] OSError in handle_send: {e}")
    finally:
        stop_event.set()  # Ensure receiver stops too

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

            stop_event = threading.Event()  # Shared event for stopping both threads

            # Start separate threads for sending and receiving
            recv_thread = threading.Thread(target=handle_receive, args=(conn, addr, stop_event), daemon=True)
            send_thread = threading.Thread(target=handle_send, args=(conn, addr, stop_event), daemon=True)

            recv_thread.start()
            send_thread.start()

            # Wait for the receive thread to finish (i.e., client disconnects)
            recv_thread.join()
            send_thread.join()

            # Now safely close conn after both threads are done
            print(f"[-] Closing connection with {addr}")
            conn.close()

if __name__ == "__main__":
    start_server()
