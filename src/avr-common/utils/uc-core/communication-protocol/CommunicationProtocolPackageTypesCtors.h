/**
 * Created by Raoul Rubien on 05.07.16
 *
 * In buffer construction of protocol package related implementation.
 */

#pragma once

#include "./CommunicationProtocol.h"
#include "./CommunicationProtocolPackageTypes.h"
#include "uc-core/particle/Globals.h"
#include "uc-core/parity/Parity.h"

/**
 * Constructor function: builds the protocol package at the given port's buffer.
 * @param txPort the port reference where to buffer the package at
 * @param localAddressRow the local address row
 * @param localAddressColumn the local address column
 */
extern PROTOCOL_PACKAGE_CTOR_ATTRS void constructEnumeratePackage(volatile TxPort *txPort,
                                                                  uint8_t localAddressRow,
                                                                  uint8_t localAddressColumn);

PROTOCOL_PACKAGE_CTOR_ATTRS void constructEnumeratePackage(volatile TxPort *txPort, uint8_t localAddressRow,
                                                           uint8_t localAddressColumn) {
    clearTransmissionPortBuffer(txPort);
    Package *package = (Package *) txPort->buffer.bytes;

    package->asEnumerationPackage.header.startBit = 1;
    package->asEnumerationPackage.header.id = PACKAGE_HEADER_ID_TYPE_ENUMERATE;
    package->asEnumerationPackage.header.isRangeCommand = false;
    package->asEnumerationPackage.header.enableBroadcast = false;

    // on local bread crumb available or origin node
    if (ParticleAttributes.protocol.hasNetworkGeometryDiscoveryBreadCrumb
        || ParticleAttributes.node.type == NODE_TYPE_ORIGIN) {
        // on tx east or tx south and no east connectivity
        if (txPort == &ParticleAttributes.communication.ports.tx.east
            || (txPort == &ParticleAttributes.communication.ports.tx.south
                && !ParticleAttributes.discoveryPulseCounters.east.isConnected)) {
            package->asEnumerationPackage.hasNetworkGeometryDiscoveryBreadCrumb = true;
        } else {
            package->asEnumerationPackage.hasNetworkGeometryDiscoveryBreadCrumb = false;
        }
    }
    package->asEnumerationPackage.addressRow = localAddressRow;
    package->asEnumerationPackage.addressColumn = localAddressColumn;
    setBufferDataEndPointer(txPort->dataEndPos, EnumerationPackageBufferPointerSize);
    setParityBit(txPort);
}

/**
 * Constructor function: builds the protocol package at the given port's buffer.
 * @param txPort the port reference where to buffer the package at
 */
extern PROTOCOL_PACKAGE_CTOR_ATTRS void constructEnumerationACKPackage(volatile TxPort *txPort);

PROTOCOL_PACKAGE_CTOR_ATTRS void constructEnumerationACKPackage(volatile TxPort *txPort) {
    clearTransmissionPortBuffer(txPort);
    Package *package = (Package *) txPort->buffer.bytes;
    package->asACKPackage.startBit = 1;
    package->asACKPackage.enableBroadcast = true;
    package->asACKPackage.id = PACKAGE_HEADER_ID_TYPE_ACK;
    package->asACKPackage.isRangeCommand = false;

    setBufferDataEndPointer(txPort->dataEndPos, AckPackagePointerSize);
    setParityBit(txPort);

}

/**
 * Constructor function: builds the protocol package at north port's buffer.
 */
extern PROTOCOL_PACKAGE_CTOR_ATTRS void constructEnumerationACKWithAddressToParentPackage(void);

PROTOCOL_PACKAGE_CTOR_ATTRS void constructEnumerationACKWithAddressToParentPackage(void) {
    clearTransmissionPortBuffer(ParticleAttributes.directionOrientedPorts.north.txPort);
    Package *package = (Package *) ParticleAttributes.directionOrientedPorts.north.txPort->buffer.bytes;
    package->asACKWithLocalAddress.header.startBit = 1;
    package->asACKWithLocalAddress.header.id = PACKAGE_HEADER_ID_TYPE_ACK_WITH_DATA;
    package->asACKWithLocalAddress.header.isRangeCommand = false;
    package->asACKWithLocalAddress.addressRow = ParticleAttributes.node.address.row;
    package->asACKWithLocalAddress.addressColumn = ParticleAttributes.node.address.column;

    setBufferDataEndPointer(ParticleAttributes.communication.ports.tx.north.dataEndPos,
                            AckWithAddressPackageBufferPointerSize);
    setParityBit(&ParticleAttributes.communication.ports.tx.north);
}

/**
 * Constructor function: builds the protocol package at the given port's buffer.
 * @param txPort the port reference where to buffer the package at
 */
extern PROTOCOL_PACKAGE_CTOR_ATTRS void constructSyncTimePackage(volatile TxPort *txPort);

PROTOCOL_PACKAGE_CTOR_ATTRS void constructSyncTimePackage(volatile TxPort *txPort) {
    clearTransmissionPortBuffer(txPort);
    Package *package = (Package *) txPort->buffer.bytes;
    package->asSyncTimePackage.header.startBit = 1;
    package->asSyncTimePackage.header.id = PACKAGE_HEADER_ID_TYPE_SYNC_TIME;
    package->asSyncTimePackage.header.isRangeCommand = false;
//    package->asSyncTimePackage.header.enableBroadcast = false;
    package->asSyncTimePackage.header.enableBroadcast = true;
    package->asSyncTimePackage.time = TIMER_TX_RX_COUNTER_VALUE;
    package->asSyncTimePackage.packageTransmissionLatency = COMMUNICATION_PROTOCOL_TIME_SYNCHRONIZATION_PACKAGE_RECEPTION_DURATION;
    package->asSyncTimePackage.stuffing1 = 0x5555;
    package->asSyncTimePackage.stuffing2 = 0x55;
    // for evaluation purpose
//    package->asSyncTimePackage.time = 0;
//    package->asSyncTimePackage.packageTransmissionLatency = 0;
//    package->asSyncTimePackage.stuffing = 0;

    setBufferDataEndPointer(txPort->dataEndPos, TimePackageBufferPointerSize);
    setParityBit(txPort);
    // DEBUG_INT16_OUT(TIMER_TX_RX_COUNTER_VALUE);
}

/**
 * Constructor function: builds the protocol package at the north port's buffer.
 * @param row the network geometry rows
 * @param column the network geometry columns
 */
extern PROTOCOL_PACKAGE_CTOR_ATTRS void constructAnnounceNetworkGeometryPackage(uint8_t row, uint8_t column);

PROTOCOL_PACKAGE_CTOR_ATTRS void constructAnnounceNetworkGeometryPackage(uint8_t row, uint8_t column) {
    clearTransmissionPortBuffer(ParticleAttributes.directionOrientedPorts.north.txPort);
    Package *package = (Package *) ParticleAttributes.directionOrientedPorts.north.txPort->buffer.bytes;
    package->asAnnounceNetworkGeometryPackage.header.startBit = 1;
    package->asAnnounceNetworkGeometryPackage.header.id = PACKAGE_HEADER_ID_TYPE_NETWORK_GEOMETRY_RESPONSE;
    package->asAnnounceNetworkGeometryPackage.header.isRangeCommand = false;
    package->asAnnounceNetworkGeometryPackage.header.enableBroadcast = true;
    package->asAnnounceNetworkGeometryPackage.rows = row;
    package->asAnnounceNetworkGeometryPackage.columns = column;

    setBufferDataEndPointer(ParticleAttributes.communication.ports.tx.north.dataEndPos,
                            AnnounceNetworkGeometryPackageBufferPointerSize);
    setParityBit(&ParticleAttributes.communication.ports.tx.north);
}


/**
 * Constructor function: builds the protocol package at the given port's buffer.
 * @param txPort the port reference where to buffer the package at
 * @param rows the new network geometry rows
 * @param columns the new network geometry columns
 */
extern PROTOCOL_PACKAGE_CTOR_ATTRS void constructSetNetworkGeometryPackage(volatile TxPort *txPort,
                                                                           uint8_t rows,
                                                                           uint8_t columns);

PROTOCOL_PACKAGE_CTOR_ATTRS void constructSetNetworkGeometryPackage(volatile TxPort *txPort, uint8_t rows,
                                                                    uint8_t columns) {
    clearTransmissionPortBuffer(txPort);
    Package *package = (Package *) txPort->buffer.bytes;
    package->asSetNetworkGeometryPackage.header.startBit = 1;
    package->asSetNetworkGeometryPackage.header.id = PACKAGE_HEADER_ID_TYPE_SET_NETWORK_GEOMETRY;
    package->asSetNetworkGeometryPackage.header.isRangeCommand = false;
    package->asSetNetworkGeometryPackage.header.enableBroadcast = true;
    package->asSetNetworkGeometryPackage.rows = rows;
    package->asSetNetworkGeometryPackage.columns = columns;

    setBufferDataEndPointer(txPort->dataEndPos, SetNetworkGeometryPackageBufferPointerSize);
    setParityBit(txPort);
}

/**
 * Constructor function: builds the protocol package at the given port's buffer.
 * Time stamp and duration units can be deduced from the local time tracking implementation.
 * For more details see {@link LocalTimeTracking} struct.
 * @param txPort the port reference where to buffer the package at
 * @param address the destination node address
 * @param wires wire flags, note: just north wires are considered
 * @param startTimeStamp the time stamp when heating starts, see also {@link LocalTimeTracking}
 * @param duration the heating period duration, see also {@link LocalTimeTracking}
 */
extern PROTOCOL_PACKAGE_CTOR_ATTRS void constructHeatWiresPackage(volatile TxPort *txPort,
                                                                  NodeAddress *address,
                                                                  Actuators *wires,
                                                                  uint16_t startTimeStamp,
                                                                  uint16_t duration);

PROTOCOL_PACKAGE_CTOR_ATTRS void constructHeatWiresPackage(volatile TxPort *txPort, NodeAddress *address,
                                                           Actuators *wires,
                                                           uint16_t startTimeStamp,
                                                           uint16_t duration) {
    clearTransmissionPortBuffer(txPort);
    Package *package = (Package *) txPort->buffer.bytes;
    package->asHeatWiresPackage.header.startBit = 1;
    package->asHeatWiresPackage.header.id = PACKAGE_HEADER_ID_TYPE_HEAT_WIRES;
    package->asHeatWiresPackage.header.isRangeCommand = false;
    package->asHeatWiresPackage.header.enableBroadcast = false;
    package->asHeatWiresPackage.addressRow = address->row;
    package->asHeatWiresPackage.addressColumn = address->column;
    package->asHeatWiresPackage.startTimeStamp = startTimeStamp;
    package->asHeatWiresPackage.duration = duration & 0x00ff;
    package->asHeatWiresPackage.durationMsb = duration & 0x0300;
    package->asHeatWiresPackage.northLeft = wires->northLeft;
    package->asHeatWiresPackage.northRight = wires->northRight;

    setBufferDataEndPointer(txPort->dataEndPos, HeatWiresPackageBufferPointerSize);
    setParityBit(txPort);
}

/**
 * Constructor function: builds the protocol package at the given port's buffer.
 * Time stamp and duration units can be deduced from the local time tracking implementation.
 * For more details see {@link LocalTimeTracking} struct.
 * @param txPort the port reference where to buffer the package at
 * @param address the destination node address
 * @param wires wire flags, note: just north wires are considered
 * @param startTimeStamp the time stamp when heating starts, see also {@link LocalTimeTracking}
 * @param duration the heating period duration, see also {@link LocalTimeTracking}
 */
extern PROTOCOL_PACKAGE_CTOR_ATTRS void constructHeatWiresRangePackage(volatile TxPort *txPort,
                                                                       NodeAddress *nodeAddressTopLeft,
                                                                       NodeAddress *nodeAddressBottomRight,
                                                                       Actuators *wires,
                                                                       uint16_t startTimeStamp,
                                                                       uint16_t duration);

PROTOCOL_PACKAGE_CTOR_ATTRS void constructHeatWiresRangePackage(volatile TxPort *txPort,
                                                                NodeAddress *nodeAddressTopLeft,
                                                                NodeAddress *nodeAddressBottomRight,
                                                                Actuators *wires,
                                                                uint16_t startTimeStamp,
                                                                uint16_t duration) {
    clearTransmissionPortBuffer(txPort);
    Package *package = (Package *) txPort->buffer.bytes;
    package->asHeatWiresRangePackage.header.startBit = 1;
    package->asHeatWiresRangePackage.header.id = PACKAGE_HEADER_ID_TYPE_HEAT_WIRES;
    package->asHeatWiresRangePackage.header.isRangeCommand = true;
    package->asHeatWiresRangePackage.header.enableBroadcast = false;
    package->asHeatWiresRangePackage.addressRow0 = nodeAddressTopLeft->row;
    package->asHeatWiresRangePackage.addressColumn0 = nodeAddressTopLeft->column;
    package->asHeatWiresRangePackage.addressRow1 = nodeAddressBottomRight->row;
    package->asHeatWiresRangePackage.addressColumn1 = nodeAddressBottomRight->column;
    package->asHeatWiresRangePackage.startTimeStamp = startTimeStamp;
    package->asHeatWiresRangePackage.duration = duration & 0x00ff;
    package->asHeatWiresRangePackage.durationMsb = (duration & 0x0300) >> 8;
    package->asHeatWiresRangePackage.northLeft = wires->northLeft;
    package->asHeatWiresRangePackage.northRight = wires->northRight;

    setBufferDataEndPointer(txPort->dataEndPos, HeatWiresRangePackageBufferPointerSize);
    setParityBit(txPort);
}

/**
 * Constructor function: builds the protocol package at the given port's buffer.
 * @param txPort the port reference where to buffer the package at
 */
extern PROTOCOL_PACKAGE_CTOR_ATTRS void constructHeaderPackage(volatile TxPort *txPort);

PROTOCOL_PACKAGE_CTOR_ATTRS void constructHeaderPackage(volatile TxPort *txPort) {
    // TODO enhancement: calculate and set parity bit
    clearTransmissionPortBuffer(txPort);
    Package *package = (Package *) txPort->buffer.bytes;
    package->asHeader.startBit = 1;
    package->asHeader.id = PACKAGE_HEADER_ID_HEADER;
    package->asHeader.isRangeCommand = false;
    package->asHeader.enableBroadcast = false;

    setBufferDataEndPointer(txPort->dataEndPos, HeaderPackagePointerSize);
    setParityBit(txPort);
}

/**
 * Constructor function: builds the protocol package at the given port's buffer.
 * @param txPort the port reference where to buffer the package at
 * @param heatingPowerLevel the power level applied to the actuators when actuating
 */
extern PROTOCOL_PACKAGE_CTOR_ATTRS void constructHeatWiresModePackage(volatile TxPort *txPort,
                                                                      HeatingLevelType heatingPowerLevel);

PROTOCOL_PACKAGE_CTOR_ATTRS void constructHeatWiresModePackage(volatile TxPort *txPort,
                                                               HeatingLevelType heatingPowerLevel) {
    clearTransmissionPortBuffer(txPort);
    Package *package = (Package *) txPort->buffer.bytes;
    package->asHeatWiresModePackage.header.startBit = 1;
    package->asHeatWiresModePackage.header.id = PACKAGE_HEADER_ID_TYPE_HEAT_WIRES_MODE;
    package->asHeatWiresModePackage.header.enableBroadcast = false;
    package->asHeatWiresModePackage.heatMode = heatingPowerLevel;

    setBufferDataEndPointer(txPort->dataEndPos, HeatWiresModePackageBufferPointerSize);
    setParityBit(txPort);
}
