
#include <stdio.h>
#include <string.h>
#include <rpc/des_crypt.h>
#include <stdint.h>
#include <malloc.h>

char seed[16] = {
        0xD5,
        0xA8,
        0xC9,
        0x1E,
        0xF5,
        0xD5,
        0x8A,
        0x23,
};


int pad_to(int len, int target) {
    int modulo = len % target;
    if (modulo == 0)
        return len;

    return len + target - modulo;
}

void dump(const char *name, const void *data, size_t size) {
    char ascii[17];
    size_t i, j;

    printf("Dump %s (%lu):\n", name, size);

    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        printf("%02X ", ((unsigned char *) data)[i]);
        if (((unsigned char *) data)[i] >= ' ' && ((unsigned char *) data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char *) data)[i];
        } else {
            ascii[i % 16] = '.';
        }

        if ((i + 1) % 8 == 0 || i + 1 == size) {
            printf(" ");
            if ((i + 1) % 16 == 0) {
                printf("|  %s \n", ascii);
            } else if (i + 1 == size) {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8) {
                    printf(" ");
                }
                for (j = (i + 1) % 16; j < 16; ++j) {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}

// Dump key:
//6D 79 64 65 76 69 63 65  73 5F 70 61 73 73 77 64  |  mydevices_passwd
//Dump data:
//73 75 70 65 72 73 65 63  72 65 74 70 61 73 73 77  |  supersecretpassw
//6F 72 64 00 00 00 00 00  00 00 00 00 00 00 00 00  |  ord.............
//
// --> results in
//
// Dump key:
//6D 79 64 65 76 69 63 65  73 5F 70 61 73 73 77 64  |  mydevices_passwd
//Dump data:
//4A 0E 5D 1A 70 4F 1F 23  72 65 74 70 61 73 73 77  |  J.].pO.#retpassw
//6F 72 64 00 00 00 00 00  00 00 00 00 00 00 00 00  |  ord.............
//
// Passphrase mydevices_passwd -> 4A 0E 5D 1A 70 4F 1F 23
//
void getHashedKey(char *keystring, int keylen, char *realkey) {
    // size_t len;
    uint64_t current_block;
    uint64_t work_key;
    char *p;
    int rest;
    int frag;

    p = keystring;
    rest = keylen;
    memcpy(&work_key, seed, 8);
    while (rest > 0) {
        frag = (rest <= 8 ? rest : 8);

        // pad with 0
        current_block = 0;
        memcpy(&current_block, p, frag);

//    printf("getHashedKey: rest=%d, frag=%d\n", rest, frag);

        p += frag;
        rest -= frag;

        work_key = work_key ^ current_block;
    }
    *(uint64_t *) realkey = work_key;
    des_setparity(realkey);
//  printf("key = %lu\n", work_key);
//  dump("Realkey", realkey, 8);
    return;
}

// Input:
// key = "mydevices_passwd" (16)
// data = "supersecretpassword" (19)
//
// Expected output:
// echo -n 8kjYaye5DsQh0epELyKNe0oZ3E3zp39X | base64 -d | hexdump -C
// 00000000  f2 48 d8 6b 27 b9 0e c4  21 d1 ea 44 2f 22 8d 7b  |.H.k'...!..D/".{|
// 00000010  4a 19 dc 4d f3 a7 7f 57                           |J..M...W|
// 00000018
//
void cbcEncrypt(char *key, int keylen, char *data, int datalen, char *out, int outlen) {
    uint64_t hashed_key;
    uint64_t ivec = 0;
    char *ciphertext = NULL;
    int ciphertext_len = 0;
    int crypt_success = 0;
    int padding;

    // prepare storage for ciphertext (with padding indicator in front
    ciphertext_len = pad_to(datalen + 4, 8);
    padding = ciphertext_len - (datalen+4);
    ciphertext = malloc(ciphertext_len);
    if (!ciphertext)
        exit(1);
    memset(ciphertext, 0, ciphertext_len);
    *(uint32_t *) ciphertext =
            (padding * 0x10000000) | 0xebb884c;
    memcpy(ciphertext + 4, data, datalen);

    dump("Key", key, keylen);
    dump("Data", data, datalen);
    dump("Ciphertext", ciphertext, ciphertext_len);
    printf("Padding: %d\n", padding);

    getHashedKey(key, keylen, (char *) &hashed_key);
    dump("hashed_key", &hashed_key, sizeof(hashed_key));

    ivec = 0;
    crypt_success = cbc_crypt((char *) &hashed_key, ciphertext, ciphertext_len, DES_ENCRYPT | DES_SW, (char *) &ivec);
    if (crypt_success != 0) {
        printf("Error: %d\n", crypt_success);
    } else {
        printf("Crypt Success!\n");
    }
    dump("Ciphertext", ciphertext, ciphertext_len);
}

int main(void) {
    char *theKey = "mydevices_passwd";
    int theKeylen = strlen(theKey);

    char *theData = "supersecretpassword";
    int theDatalen = strlen(theData);

    char *out = malloc(80);
    int outlen = 79;

    cbcEncrypt(theKey, theKeylen, theData, theDatalen, out, outlen);
}
