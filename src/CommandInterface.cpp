#include "CommandInterface.h"

uint8_t CommandInterface::begin(void (*onSetSegmentsPtr)(cmd_set_segments&))
{
    this->onSetSegmentsPtr = onSetSegmentsPtr;

    // initialize the transceiver on the SPI bus
    if (!radio.begin())
        return 0;

    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to
    // each other.
    radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

    radio.setPayloadSize(32); // float datatype occupies 4 bytes

    // Pipes are used to logically divide multiple senders on the same radio channel/frequency.
    // An address is linked to a pipe. This way it is much easier to filter messages
    // from different sender addresses.
    // We don't really use this feature as only one sender is used in this application.
    // So just assign the sender to pipe 0. The other pipes are not used.
    // uint8_t other_node_addr[6] = "2Node";
    uint8_t other_node_addr[6] = "B2TFR"; // Back To The Future Remote
    radio.openReadingPipe(1, other_node_addr); // pipe 1 = other_node_addr

    radio.startListening();

    return 1;
}

void CommandInterface::handleInput()
{
    if (radio.failureDetected) 
    {
        radio.failureDetected = false;
        delay(250);
        begin(onSetSegmentsPtr);
    }

    uint8_t payload[32] = {0};

    uint8_t pipe;
    if (radio.available(&pipe)) { // Available? On which pipe was it received?
        radio.read(payload, 32); // fetch payload from FIFO

        if( payload[0] != 'B' || 
            payload[1] != '2' ||
            payload[2] != 'T' ||
            payload[3] != 'F')
            return;

        switch (payload[4])
        {
        case CMD_SET_SEGMENTS:
        {
            cmd_set_segments& cmd = reinterpret_cast<cmd_set_segments&>(payload);

            if(cmd.length > 25)
                break;

            onSetSegmentsPtr(cmd);
            break;
        }
        case CMD_PLAY_SOUND:
            Serial.print("CMD_PLAY_SOUND ");
            break;

        case CMD_DISABLE_DCF77:
            Serial.print("CMD_DISABLE_DCF77 ");
            break;

        default:
            Serial.print("Unknown command ");
            break;
        }
    }

    // TODO: call callback when data received. Outside function should handle data.
}