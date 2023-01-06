from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives import serialization

# create a private EC key
private_key = ec.generate_private_key(ec.SECP256R1()) 
private_pem = private_key.private_bytes(
    encoding=serialization.Encoding.PEM,
    format=serialization.PrivateFormat.TraditionalOpenSSL,
    encryption_algorithm=serialization.NoEncryption()
)
print(private_pem.decode("utf-8"))

# get private key value
private_key_bytes = bytes.fromhex(format(private_key.private_numbers().private_value, '064x')) #converts int to hex
list_of_bytes = list(map(hex, list(private_key_bytes)))     # creates list of bytes
ESP_private_key = ",".join(list_of_bytes)                   # comma separates each byte
print(f"Device key:\n {ESP_private_key}\n")

# generate public EC key from private key
public_key = private_key.public_key()
public_pem = public_key.public_bytes(
    encoding=serialization.Encoding.PEM,
    format=serialization.PublicFormat.SubjectPublicKeyInfo)
print(public_pem.decode("utf-8"))