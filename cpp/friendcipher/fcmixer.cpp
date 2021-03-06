/*
friendcipher::MixWithRng
Copyright (C) 2016 OnlineWolf

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

URL: https://github.com/onlinewolf/friendcipher
*/
#include <cmath>
#include "fcmixer.h"
#include "fcexception.h"

namespace friendcipher{

//class
MixWithRng::MixWithRng(int bitLen):
            rng_(bitLen), hash_(bitLen), key_(nullptr), iv_(nullptr), kMdLen(bitLen/8), kMdBitLen(bitLen){
    if(!keccakBitLenCheck(bitLen))
        throw invalidArgsException;

    temp_ = new uint8_t[kMdLen];
    listTemp_ = new int[kMdLen];
    init_ = false;

}

bool MixWithRng::init(const uint8_t *key, int keyLen, const uint8_t *iv, int ivLen){
    if(!key || keyLen <= 0 || !iv || ivLen <= 0)
        return false;

    key_ = key;
    iv_ = iv;
    keyLen_ = keyLen;
    ivLen_ = ivLen;
    init_ = true;
    return true;
}

bool MixWithRng::isInited(){
    return init_;
}

void MixWithRng::listMix(uint8_t* tempIn, uint8_t* dataOut, uint8_t len){
    if(!tempIn || !dataOut || len==0)
        return;

    uint8_t random;
    for(uint8_t i=0, mlen=len; i<len; i++, mlen--){
        random = rng_.random8bit() % mlen;
        dataOut[i] = tempIn[random];
        tempIn[random] = tempIn[mlen-1];
    }
}

void MixWithRng::listReverseMix(uint8_t* tempIn, uint8_t* dataOut, uint8_t len){
    if(!tempIn || !dataOut || len==0)
        return;

    for(int i=0; i<kMdLen; i++)
        listTemp_[i] = i;

    uint8_t random;
    for(uint8_t i=0, mlen=len; i<len; i++, mlen--){
        random = rng_.random8bit() % mlen;
        dataOut[listTemp_[random]] = tempIn[i];
        listTemp_[random] = listTemp_[mlen-1];
    }
}


bool MixWithRng::mix(const uint8_t* dataIn, uint8_t *dataOut, int len, uint32_t counter){
    if(!dataIn || !dataOut || len <= 0 || !init_)
        return false;

    rng_.init(key_, keyLen_, nullptr, 0);
    hash_.update(iv_, ivLen_);
    if(counter != 0)
        hash_.update((uint8_t *)&counter, 4);
    hash_.finish(temp_);
    rng_.reSeed(temp_, kMdLen);

    uint8_t blockLen = calcBlockSize(rng_.random8bit(), kMdLen);
    uint8_t reCount = 0;
    for(int i=0; i<len; i++){
        temp_[reCount] = dataIn[i];
        reCount++;
        if(reCount > blockLen){
            listMix(temp_, &dataOut[i-blockLen], reCount);
            reCount = 0;
            if(i!=(len-1)){
                blockLen = calcBlockSize(rng_.random8bit(), kMdLen);
            }
        }
    }

    if(reCount!=0){
        listMix(temp_, &dataOut[len-reCount], reCount);
    }

    return true;
}

bool MixWithRng::reverseMix(const uint8_t* dataIn, uint8_t *dataOut, int len, uint32_t counter){
    if(!dataIn || !dataOut || len <= 0 || !init_)
        return false;

    rng_.init(key_, keyLen_, nullptr, 0);
    hash_.update(iv_, ivLen_);
    if(counter != 0)
        hash_.update((uint8_t *)&counter, 4);
    hash_.finish(temp_);
    rng_.reSeed(temp_, kMdLen);

    uint8_t blockLen = calcBlockSize(rng_.random8bit(), kMdLen);
    uint8_t reCount = 0;
    for(int i=0; i<len; i++){
        temp_[reCount] = dataIn[i];
        reCount++;
        if(reCount > blockLen){
            listReverseMix(temp_, &dataOut[i-blockLen], reCount);
            reCount = 0;
            if(i!=(len-1)){
                blockLen = calcBlockSize(rng_.random8bit(), kMdLen);
            }
        }
    }

    if(reCount!=0){
        listReverseMix(temp_, &dataOut[len-reCount], reCount);
    }

    return true;
}

bool MixWithRng::crazyMix(const uint8_t *dataIn, uint8_t *dataOut, int len){
    if(!dataIn || !dataOut || len <= 0 || !init_)
        return false;


    rng_.init(key_, keyLen_, iv_, ivLen_);

    const uint8_t *temp = dataIn;
    int crazyNumber = calcCrazy(rng_.random8bit(), kMinCrazy, kMaxCrazy) & 0xFF;

    if(crazyNumber % 2 != 1)
        crazyNumber++;

    for(int i=0; i<crazyNumber; i++){
        if(!mix(temp, dataOut, len, i))
            return false;
        if(i == 0){
            temp = dataOut;
        }
    }
    return true;
}

bool MixWithRng::reverseCrazyMix(const uint8_t *dataIn, uint8_t *dataOut, int len){
    if(!dataIn || !dataOut || len <= 0 || !init_)
        return false;

    rng_.init(key_, keyLen_, iv_, ivLen_);

    const uint8_t *temp = dataIn;
    int crazyNumber = calcCrazy(rng_.random8bit(), kMinCrazy, kMaxCrazy) & 0xFF;

    if(crazyNumber % 2 != 1)
        crazyNumber++;

    for(int i=crazyNumber-1; i>=0; i--){
        if(!reverseMix(temp, dataOut, len, i))
            return false;
        if(i == crazyNumber-1){
            temp = dataOut;
        }
    }
    return true;
}

MixWithRng::~MixWithRng(){
    delete[] temp_;
    delete[] listTemp_;
}


//"static" functions
uint8_t calcBlockSize(uint8_t x, uint8_t bmax){
    if(bmax<=1)
        return 0;

    uint8_t bmin = bmax/2;
    x = x % bmin;
    return x + bmin;
}

int calcConvert(int x, double xmax, int min, int max){
    if(x<0 || xmax<=0 || min<0 || max<=1)
        return 0;

    if(x>xmax)
        return 0;

    if(min>max){
        int temp = min;
        min = max;
        max = temp;
    }

    return std::lround((x/xmax * (max-min-1)) + min);
}


uint8_t calcCrazy(uint8_t x, uint8_t min, uint8_t max){
    if(max <= 1)
        return 1;

    if(min == 0)
        min = 1;

    if(min>max){
        uint8_t temp = min;
        min = max;
        max = temp;
    }

    x = x % (max - min);
    return x + min;
}

}
