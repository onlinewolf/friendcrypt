#ifndef FCKECCAK_H
#define FCKECCAK_H
#include <cstdint>

namespace friendcrypt{

//Intel x86, x86-64
#define LITTLE_ENDIAN

/**
 * @brief The Keccak class
 * Keccak C++ implementation with update()
 */
class Keccak{
    uint8_t state_[200];
    long rateInBytes_;
    static const uint8_t kDelimitedSuffix_ = 0x06;
    uint8_t* forUpdate_;
    long updatePos_;
    long mdLen_;
    void reset();
public:
    /**
     * @brief Keccak
     * Message digest bit length: 224, 256, 384, 512
     * @param mdBitLen Message digest bit length
     * @throw invalidArgsException if mdBitLen is incorrect
     */
    explicit Keccak(long mdBitLen);
    /**
     * @brief update
     * Add new data
     * @param data Data
     * @param len Length
     */
    void update(const uint8_t *data, long len);
    /**
     * @brief finish
     * Finish and reset
     * @param out Output array (length of array is mdLen)
     */
    void finish(uint8_t *out);

    /**
     * @brief ~Keccak
     * Delete forUpdate_
     */
    virtual ~Keccak();

    //disabled
    Keccak(const Keccak& other)=delete;
    Keccak(Keccak&& other)=delete;
    Keccak& operator=(const Keccak& other)=delete;
    Keccak& operator=(Keccak&& other)=delete;
};


}//namespace
#endif // FCKECCAK_H