import ssl
import socket
import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument('-k', help='directory with keys')
parser.add_argument('-b', help='binary to flash')
parser.add_argument('-a', help='host address')
args = parser.parse_args()

print (args)

ca_cert = os.path.join(args.k, "ca_cert.pem")
my_cert = os.path.join(args.k, "cl_cert.pem")
my_key = os.path.join(args.k, "cl_key.pem")

context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
context.load_verify_locations(ca_cert)
context.load_cert_chain(my_cert, my_key)
context.check_hostname = False
context.verify_mode = ssl.CERT_NONE
# context.


with open(args.b, 'rb') as binary:
    with socket.create_connection((args.a, 60000)) as sock:
        with context.wrap_socket(sock) as ssock:
            while True:
                chunk = binary.read(256)
                if not chunk:
                    break
                ssock.write(chunk)
                print('.', end='', flush=True)
                
            