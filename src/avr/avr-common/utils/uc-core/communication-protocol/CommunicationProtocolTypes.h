//
// Created by Raoul Rubien on 11.05.16.
//

/**
 * definition of all package types that can be transmitted/received
 */
#ifndef __COMMUNICATION_PROTOCOL_TYPES_H__
#define __COMMUNICATION_PROTOCOL_TYPES_H__

#  ifdef TRY_INLINE
#    define FUNC_ATTRS inline
#  else
#    define FUNC_ATTRS
#  endif

/**
 * describes a package header
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
} PackageHeader;

/**
 * PackageHeader data length expressed as BufferPointer
 */
#define PackageHeaderBufferPointerSize ((BufferBitPointer) {.byteNumber = 0, .bitMask = (1 << 7)})

/**
 * describes a package header with subsequent address
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
    uint8_t addressRow0 : 8;
    uint8_t addressColumn0 : 8;
} PackageHeaderAddress;

/**
 * PackageHeaderAddress length expressed as BufferPointer
 */
#define PackageHeaderAddressBufferPointerSize ((BufferBitPointer) {.byteNumber = 2, .bitMask = (1 << 7)})

/**
 * describes a package header with subsequent address range
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
    uint8_t addressRow0 : 8;
    uint8_t addressColumn0 : 8;
    uint8_t addressRow1 : 8;
    uint8_t addressColumn1 : 8;
} PackageHeaderAddressRange;

/**
 * PackageHeaderAddressRange length expressed as BufferPointer
 */
#define PackageHeaderAddressRangeBufferPointerSize ((BufferBitPointer){.byteNumber = 4, .bitMask = (1 << 7)})

/**
 * describes a package header with 7 subsequent data bits
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
    uint8_t dataLsb : 7;
} PackageHeaderData7;

/**
 * PackageHeaderData7 length expressed as BufferPointer
 */
#define PackageHeaderData7BufferPointerSize ((BufferBitPointer){.byteNumber = 1, .bitMask = (1 << 6)})

/**
 * describes a package header with 11 subsequent data bits
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
    uint8_t dataLsb : 8;
    uint8_t dataMsb : 3;
} PackageHeaderData11;

/**
 * PackageHeaderData11 length expressed as BufferPointer
 */
#define PackageHeaderData11BufferPointerSize ((BufferBitPointer) {.byteNumber = 2, .bitMask = (1 << 2)})

/**
 * describes a package header with 19 subsequent data bits
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
    uint8_t dataLsb : 8;
    uint8_t dataCeb : 8;
    uint8_t dataMsb : 3;
} PackageHeaderData19;

/**
 * PackageHeaderData17 length expressed as BufferPointer
 */
#define PackageHeaderData19BufferPointerSize ((BufferBitPointer) {.byteNumber = 3, .bitMask = (1 << 2)})

/**
 * describes a package header with subsequent address and seven data bits
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
    uint8_t addressRow0 : 8;
    uint8_t addressColumn0 : 8;
    uint8_t dataLsb : 7;
} PackageHeaderAddressData7;

/**
 * PackageHeaderAddressData7 length expressed as BufferPointer
 */
#define PackageHeaderAddressData7BufferPointerSize ((BufferBitPointer){.byteNumber = 3, .bitMask = (1 << 6)})

/**
 * describes a package header with subsequent address and 11 data bits
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
    uint8_t addressRow0 : 8;
    uint8_t addressColumn0 : 8;
    uint8_t dataMsb : 3;
} PackageHeaderAddressData11;

/**
 * PackageHeaderAddressData11 length expressed as BufferPointer
 */
#define PackageHeaderAddressData11BufferPointerSize ((BufferBitPointer){.byteNumber = 3, .bitMask = (1 << 2)})

/**
 * describes a package header with subsequent address and 19 data bits
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
    uint8_t addressRow0 : 8;
    uint8_t addressColumn0 : 8;
    uint8_t dataLsb : 8;
    uint8_t dataCeb : 8;
    uint8_t dataMsb : 3;
} PackageHeaderAddressData19;

/**
 * PackageHeaderAddressData19 length expressed as BufferPointer
 */
#define PackageHeaderAddressData19BufferPointerSize ((BufferBitPointer){.byteNumber = 4, .bitMask = (1 << 2)})

/**
 * describes a package header with subsequent address range and 7 data bits
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
    uint8_t addressRow0 : 8;
    uint8_t addressColumn0 : 8;
    uint8_t addressRow1 : 8;
    uint8_t addressColumn1 : 8;
    uint8_t dataLsb : 7;
} PackageHeaderAddressRangeData7;

/**
 * PackageHeaderAddressRangeData7 length expressed as BufferPointer
 */
#define PackageHeaderAddressRangeData7BufferPointerSize ((BufferBitPointer){.byteNumber = 5, .bitMask = (1 << 6)})

/**
 * describes a package header with subsequent address range and 11 data bits
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
    uint8_t addressRow0 : 8;
    uint8_t addressColumn0 : 8;
    uint8_t addressRow1 : 8;
    uint8_t addressColumn1 : 8;
    uint8_t dataLsb : 8;
    uint8_t dataMsb : 3;
} PackageHeaderAddressRangeData11;

/**
 * PackageHeaderAddressRangeData11 length expressed as BufferPointer
 */
#define PackageHeaderAddressRangeData11BufferPointerSize ((BufferBitPointer){.byteNumber = 6, .bitMask = (1 << 2)})

/**
 * describes a package heaer with subsequent address range and 19 data bits
 */
typedef struct {
    uint8_t headerIsStream : 1;
    uint8_t headerIsCommand : 1;
    uint8_t headerIsBroadcast : 1;
    uint8_t headerIsReserved : 1;
    uint8_t headerId : 4;
    uint8_t addressRow0 : 8;
    uint8_t addressColumn0 : 8;
    uint8_t addressRow1 : 8;
    uint8_t addressColumn1 : 8;
    uint8_t dataLsb : 8;
    uint8_t dataCeb : 8;
    uint8_t dataMsb : 3;
} PackageHeaderAddressRangeData19;

/**
 * PackageHeaderAddressRangeData19 length expressed as BufferPointer
 */
#define PackageHeaderAddressRangeData19BufferPointerSize ((BufferBitPointer){.byteNumber = 7, .bitMask = (1 << 2)})

typedef union {
    PackageHeader asBroadcastHeader;
    PackageHeaderAddress asDedicatedHeader;
    PackageHeaderAddressRange asMulticastHeader;

    PackageHeaderData7 asBroadcastData7;
    PackageHeaderAddressData7 asDedicatedData7;
    PackageHeaderAddressRangeData7 asMulticastData7;

    PackageHeaderData11 asBroadcastData11;
    PackageHeaderAddressData11 asDedicatedData11;
    PackageHeaderAddressRangeData11 asMulticastData11;

    PackageHeaderData19 asBroadcastData19;
    PackageHeaderAddressData19 asDedicatedData19;
    PackageHeaderAddressRangeData19 asMulticastData19;
} Package;

#  ifdef FUNC_ATTRS
#    undef FUNC_ATTRS
#  endif

#endif

