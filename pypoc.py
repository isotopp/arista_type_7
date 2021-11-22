#! /usr/bin/env python3 

from Crypto.Cipher import DES
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
import base64

target = b'AEeZkeT2qaavxk5/S9QDbUpTTtf7PgqU'
target_padded_cleartext = bytearray.fromhex('4C 88 BB 4E 74 65 73 74 74 65 73 74 74 65 73 74 74 65 73 74 00 00 00 00')
key = b'TEST_passwd'
cleartext = b'testtesttesttest'

SEED = b"\xd5\xa8\xc9\x1e\xf5\xd5\x8a\x23"

PARITY_BITS = [
    0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x07, 0x07,
    0x08, 0x08, 0x0b, 0x0b, 0x0d, 0x0d, 0x0e, 0x0e,
    0x10, 0x10, 0x13, 0x13, 0x15, 0x15, 0x16, 0x16,
    0x19, 0x19, 0x1a, 0x1a, 0x1c, 0x1c, 0x1f, 0x1f,
    0x20, 0x20, 0x23, 0x23, 0x25, 0x25, 0x26, 0x26,
    0x29, 0x29, 0x2a, 0x2a, 0x2c, 0x2c, 0x2f, 0x2f,
    0x31, 0x31, 0x32, 0x32, 0x34, 0x34, 0x37, 0x37,
    0x38, 0x38, 0x3b, 0x3b, 0x3d, 0x3d, 0x3e, 0x3e,
    0x40, 0x40, 0x43, 0x43, 0x45, 0x45, 0x46, 0x46,
    0x49, 0x49, 0x4a, 0x4a, 0x4c, 0x4c, 0x4f, 0x4f,
    0x51, 0x51, 0x52, 0x52, 0x54, 0x54, 0x57, 0x57,
    0x58, 0x58, 0x5b, 0x5b, 0x5d, 0x5d, 0x5e, 0x5e,
    0x61, 0x61, 0x62, 0x62, 0x64, 0x64, 0x67, 0x67,
    0x68, 0x68, 0x6b, 0x6b, 0x6d, 0x6d, 0x6e, 0x6e,
    0x70, 0x70, 0x73, 0x73, 0x75, 0x75, 0x76, 0x76,
    0x79, 0x79, 0x7a, 0x7a, 0x7c, 0x7c, 0x7f, 0x7f
]


# print(base64.b64encode(bytearray.fromhex('00 47 99 91 E4 F6 A9 A6 AF C6 4E 7F 4B D4 03 6D 4A 53 4E D7 FB 3E 0A 94')))


def phex(data):
    print(" ".join("{:02x}".format(c) for c in data))


def des_setparity(key):
    res = b""
    for b in key:
        pos = b & 0x7f
        res += PARITY_BITS[pos].to_bytes(1, byteorder='big')
    return res

def hashKey(pw):
    result = bytearray(SEED)

    for idx, b in enumerate(pw):
        result[idx & 7] ^= b

    print('Hashed key - without parity set:')
    phex(result)
    result = des_setparity(result)
    print('Hashed key - WITH parity set:')
    phex(result)

    return bytes(result)


def cbcEncrypt(key: bytes, data: bytes):
    hashed_key = hashKey(key)
    print('Cleartext data:')
    phex(data)
    padding = (8 - ((len(data) + 4) % 8)) % 8
    ciphertext = b"\x4c\x88\xbb" + bytes([padding * 16 + 0xe]) + data + bytes(padding)
    print('Padded ciphertext:')
    phex(ciphertext)
    assert ciphertext == target_padded_cleartext, 'Wrong padded cleartext'
    cipher = DES.new(hashed_key, DES.MODE_CBC, 8 * b'\x00')
    print('Used IV:')
    phex(cipher.iv)
    result = cipher.encrypt(ciphertext)
    print('Result:')
    phex(result)
    return (result)

def cbcEncrypt2(key: bytes, data: bytes):
    hashed_key = hashKey(key)
    print('Cleartext data:')
    phex(data)
    padding = (8 - ((len(data) + 4) % 8)) % 8
    ciphertext = b"\x4c\x88\xbb" + bytes([padding * 16 + 0xe]) + data + bytes(padding)
    print('Padded ciphertext:')
    phex(ciphertext)
    assert ciphertext == target_padded_cleartext, 'Wrong padded cleartext'
    cipher = Cipher(algorithms.TripleDES(hashed_key), modes.CBC(8 * b'\x00'), default_backend())
    print('Used IV:')
    phex(cipher.mode.initialization_vector)
    encryptor = cipher.encryptor()
    result = encryptor.update(ciphertext)
    encryptor.finalize()

    print('Result:')
    phex(result)
    return (result)

if __name__ == '__main__':
    for func in [cbcEncrypt, cbcEncrypt2]:
        print(f"--> Calling {func}")
        result = base64.b64encode(func(key, cleartext))
        print(result)
        if result != target:
            print('Not the expected output :(')
        else:
            print('WE DID IT!')

