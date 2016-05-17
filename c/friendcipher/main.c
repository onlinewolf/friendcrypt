#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "fccipher.h"
#include "fcspeed.h"


#define MD_BIT_LEN 512
#define MD_LEN MD_BIT_LEN/8

#define DATA_LEN 32*1024
#define IV_LEN 16
#define IV_BITLEN IV_LEN*8
#define KEY_LEN 16
#define KEY_BITLEN KEY_LEN*8

void convert(uint64_t x){
    if(x > 1024LL*1024LL*1024LL)
        printf("%llu GBps", x/(1024LL*1024LL*1024LL));
    else if(x > 1024LL*1024LL)
        printf("%llu MBps", x/(1024LL*1024LL));
    else if(x > 1024)
        printf("%llu kBps", x/(1024));
    else
        printf("%llu Bps", x);
}


int main(){

    //data
    uint8_t *data = (uint8_t*)malloc(DATA_LEN);
    int i;
    for(i=0; i<DATA_LEN; i++){
        data[i] = i;
    }

    uint8_t *dataOut = (uint8_t*)malloc(DATA_LEN);
    uint8_t *dataOut2 = (uint8_t*)malloc(DATA_LEN);

    //key
    uint8_t *key = (uint8_t*)malloc(KEY_LEN);
    for(i=0; i<KEY_LEN; i++){
        key[i] = i;
    }

    //iv
    uint8_t *iv = (uint8_t*)malloc(IV_LEN);
    for(i=0; i<IV_LEN; i++){
        iv[i] = i;
    }


    printf("(Hash) Data length: %d\n", DATA_LEN);
    printf("224 bit: "); convert(hashSpeed(224, data, DATA_LEN, dataOut)); printf("\n");
    printf("256 bit: "); convert(hashSpeed(256, data, DATA_LEN, dataOut)); printf("\n");
    printf("384 bit: "); convert(hashSpeed(384, data, DATA_LEN, dataOut)); printf("\n");
    printf("512 bit: "); convert(hashSpeed(512, data, DATA_LEN, dataOut)); printf("\n");
    printf("\n\n");

    printf("(RNG) Data length: %d; key: %d bit; iv: %d bit\n", DATA_LEN, KEY_BITLEN, IV_BITLEN);
    printf("224 bit: "); convert(rngSpeed(224, key, KEY_LEN, iv, IV_LEN, dataOut, DATA_LEN)); printf("\n");
    printf("256 bit: "); convert(rngSpeed(256, key, KEY_LEN, iv, IV_LEN, dataOut, DATA_LEN)); printf("\n");
    printf("384 bit: "); convert(rngSpeed(384, key, KEY_LEN, iv, IV_LEN, dataOut, DATA_LEN)); printf("\n");
    printf("512 bit: "); convert(rngSpeed(512, key, KEY_LEN, iv, IV_LEN, dataOut, DATA_LEN)); printf("\n");
    printf("\n\n");

    printf("(Encrypt) Data length: %d; key: %d bit; iv: %d bit\n", DATA_LEN, KEY_BITLEN, IV_BITLEN);
    printf("224 bit: "); convert(cipherSpeed(1, 224, data, dataOut, DATA_LEN, key, KEY_LEN, iv, IV_LEN)); printf("\n");
    printf("256 bit: "); convert(cipherSpeed(1, 256, data, dataOut, DATA_LEN, key, KEY_LEN, iv, IV_LEN)); printf("\n");
    printf("384 bit: "); convert(cipherSpeed(1, 384, data, dataOut, DATA_LEN, key, KEY_LEN, iv, IV_LEN)); printf("\n");
    printf("512 bit: "); convert(cipherSpeed(1, 512, data, dataOut, DATA_LEN, key, KEY_LEN, iv, IV_LEN)); printf("\n");
    printf("\n\n");

    printf("(Decrypt) Data length: %d; key: %d bit; iv: %d bit\n", DATA_LEN, KEY_BITLEN, IV_BITLEN);
    printf("224 bit: "); convert(cipherSpeed(0, 224, data, dataOut, DATA_LEN, key, KEY_LEN, iv, IV_LEN)); printf("\n");
    printf("256 bit: "); convert(cipherSpeed(0, 256, data, dataOut, DATA_LEN, key, KEY_LEN, iv, IV_LEN)); printf("\n");
    printf("384 bit: "); convert(cipherSpeed(0, 384, data, dataOut, DATA_LEN, key, KEY_LEN, iv, IV_LEN)); printf("\n");
    printf("512 bit: "); convert(cipherSpeed(0, 512, data, dataOut, DATA_LEN, key, KEY_LEN, iv, IV_LEN)); printf("\n");
    printf("\n\n");

/*
    fc_cipher_t cipher;
    fc_cipher_init(&cipher, MD_BIT_LEN);
    fc_cipher_setIv(&cipher, iv, IV_LEN);
    fc_cipher_setKey(&cipher, key, KEY_LEN);
    fc_cipher_encrypt(&cipher, data, dataOut, DATA_LEN);
    fc_cipher_decrypt(&cipher, dataOut, dataOut2, DATA_LEN);

    for(i=0; i<DATA_LEN; i++){
        printf("%d, ", dataOut[i]);
    }
    printf("\n\n");

    for(i=0; i<DATA_LEN; i++){
        printf("%d, ", dataOut2[i]);
    }
    printf("\n\n");
*/


    //Hash
    fc_hash_t keccak;
    fc_hashInit(&keccak, MD_BIT_LEN);
    uint8_t data1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40};
    uint8_t data2[] = {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80};

    uint8_t mDigest[MD_LEN];
    fc_hashUpdate(&keccak, data1, 40);
    fc_hashUpdate(&keccak, data2, 40);
    fc_hashFinish(&keccak, mDigest);
    //show
    printf("Hash: ");
    for(i=0; i<MD_LEN; i++){
        printf("%02X", mDigest[i]);
    }
    printf("\n\n");

/*
    //RNG
    fc_rng_t rng;
    FC_RNG_INIT(&rng, MD_BIT_LEN);

    time_t ti = time(NULL);
    FC_RNG_SEED(&rng, (uint8_t*)&ti, sizeof(ti), NULL, 0);

    uint32_t random;
    FC_RNG_RANDOM8(&rng, random);
    printf("RANDOM8: %u\n", random);
    FC_RNG_RANDOM32(&rng, random);
    printf("RANDOM32: %u\n\n", random);

    FC_RNG_SEED(&rng, data1, 40, NULL, 0);
    FC_RNG_RANDOM8(&rng, random);
    printf("SRANDOM8: %u\n", random);
    FC_RNG_RANDOM32(&rng, random);
    printf("SRANDOM32: %u\n", random);

    FC_RNG_SEED(&rng, data1, 40, data2, 40);
    FC_RNG_RANDOM8(&rng, random);
    printf("S2RANDOM8: %u\n", random);
    FC_RNG_RANDOM32(&rng, random);
    printf("S2RANDOM32: %u\n", random);

    FC_RNG_RESEED(&rng, data1, 40);
    FC_RNG_RANDOM8(&rng, random);
    printf("RRANDOM8: %u\n", random);
    FC_RNG_RANDOM32(&rng, random);
    printf("RRANDOM32: %u\n", random);
*/

    free(data);
    free(dataOut);
    free(dataOut2);
    free(key);
    free(iv);

    return 0;
}
