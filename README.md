# arista_type_7

Generate Arista Type 7 Passwords in C

A friend had the need to provision Arista Type 7 Passwords on switches.
According to 
[Ryan Gelobter](https://medium.com/@what_if/encrypting-decrypting-arista-bgp-bmp-ospf-passwords-ff2072460942),
Arista has a Python library to do this.

Apparently this Python Library uses a Cython Module to provide the functions 
- ` _DesCrypt.cbcEncrypt( key, data )`
- `_DesCrypt.cbcDecrypt( key, data )`

Unfortunately, this code does not run on a regular Linux machine, due to it being 32 bit, for an outdated version of Python (3.7) and dependencies.
To make provisioning easier, I reversed the function and provide an independent PoC C implementation of the encryption function.

This now can be turned into a pure Python function.

# Build

This has been tested on Ubuntu.

I had to install

- libtirpc-common
- libtirpc-dev
- libtirpc3

to get access to `<rpc/des_encrypt>` and I had to link `-static` to make it work. For a PoC that is acceptable.

# Validation

This has been validated only with the test data provided in the Ryan Gelobter article above.
