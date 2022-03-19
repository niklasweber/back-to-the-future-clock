#include "CommandInterface.h"

// uint8_t CommandInterface::begin(void (*onSetSegmentsPtr)(cmd_set_segments&),
//                                 void (*onShowTimePtr)(cmd_show_time&))
// {
//     this->onSetSegmentsPtr = onSetSegmentsPtr;
//     this->onShowTimePtr = onShowTimePtr;

//     // initialize the transceiver on the SPI bus
//     if (!radio.begin())
//         return 0;

//     // Set the PA Level low to try preventing power supply related problems
//     // because these examples are likely run with nodes in close proximity to
//     // each other.
//     radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

//     radio.setPayloadSize(32); // float datatype occupies 4 bytes

//     // Pipes are used to logically divide multiple senders on the same radio channel/frequency.
//     // An address is linked to a pipe. This way it is much easier to filter messages
//     // from different sender addresses.
//     // We don't really use this feature as only one sender is used in this application.
//     // So just assign the sender to pipe 0. The other pipes are not used.
//     // uint8_t other_node_addr[6] = "2Node";
//     uint8_t other_node_addr[6] = "B2TFR"; // Back To The Future Remote
//     radio.openReadingPipe(1, other_node_addr); // pipe 1 = other_node_addr

//     radio.startListening();

//     return 1;
// }

// void CommandInterface::handleInput()
// {
//     if (radio.failureDetected) 
//     {
//         radio.failureDetected = false;
//         delay(250);
//         begin(onSetSegmentsPtr, onShowTimePtr);
//     }

//     uint8_t payload[32] = {0};

//     uint8_t pipe;
//     if (radio.available(&pipe)) { // Available? On which pipe was it received?
//         radio.read(payload, 32); // fetch payload from FIFO

//         if( payload[0] != 'B' || 
//             payload[1] != '2' ||
//             payload[2] != 'T' ||
//             payload[3] != 'F')
//             return;

//         switch (payload[4])
//         {
//         case CMD_SET_SEGMENTS:
//         {
//             cmd_set_segments& cmd = reinterpret_cast<cmd_set_segments&>(payload);

//             if(cmd.length > 25)
//                 break;

//             onSetSegmentsPtr(cmd);
//             break;
//         }
//         case CMD_CONTROL_SOUND:
//         {
//             // Serial.print("CMD_PLAY_SOUND ");
//             break;
//         }
//         case CMD_SHOW_TIME:
//         {
//             cmd_show_time& cmd = reinterpret_cast<cmd_show_time&>(payload);
//             onShowTimePtr(cmd);
//             break;
//         }
//         }
//     }
// }

// bool deviceConnected = false;

void CommandInterface::onConnect(BLEServer* pServer)
{
    // deviceConnected = true;
    //BLEDevice::startAdvertising();
    Serial.println("Client connected");
}

void CommandInterface::onDisconnect(BLEServer* pServer)
{
    // deviceConnected = false;
    Serial.println("Client disconnected");
}

void CommandInterface::onWrite(BLECharacteristic* pCharacteristic)
{
    std::string uuid = pCharacteristic->getUUID().toString();
    std::string data = pCharacteristic->getValue();
    
    if(uuid.compare(SEGMENT_CHARACTERISTIC_UUID) == 0)
    {
        onSetSegmentPtr(data);
    }
    else if(uuid.compare(SHOW_TIME_CHARACTERISTIC_UUD) == 0)
    {
        onShowTimePtr(data);
    }
    else if(uuid.compare(BRIGHTNESS_CHARACTERISTIC_UUID) == 0)
    {
        onSetBrightnessPtr(data);
    }
    else if(uuid.compare(TIME_CHARACTERISTIC_UUID) == 0)
    {
        onSetTimePtr(data);
    }
    else if(uuid.compare(VOLUME_CHARACTERISTIC_UUID) == 0)
    {
        onSetVolumePtr(data);
    }
    else if(uuid.compare(PLAYBACK_CHARACTERISTIC_UUID) == 0)
    {
        onSetPlaybackPtr(data);
    }
}

uint8_t CommandInterface::begin()
{
    BLEDevice::init("B");
    BLEServer *pServer = BLEDevice::createServer();

    BLEService *pDisplayService = pServer->createService(DISPLAY_SERVICE_UUID);
    BLECharacteristic *pSegmentChar = pDisplayService->createCharacteristic(
                                            SEGMENT_CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_WRITE
                                        );
    BLECharacteristic *pShowTimeChar = pDisplayService->createCharacteristic(
                                            SHOW_TIME_CHARACTERISTIC_UUD,
                                            BLECharacteristic::PROPERTY_WRITE
                                        );
    BLECharacteristic *pBrightnessChar = pDisplayService->createCharacteristic(
                                            BRIGHTNESS_CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_WRITE
                                        );

    BLEService *pTimeService = pServer->createService(TIME_SERVICE_UUID);
    BLECharacteristic *pTimeChar = pTimeService->createCharacteristic(
                                            TIME_CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_WRITE
                                        );

    BLEService *pSoundService = pServer->createService(SOUND_SERVICE_UUID);
    BLECharacteristic *pVolumeChar = pSoundService->createCharacteristic(
                                            VOLUME_CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_WRITE
                                        );
    BLECharacteristic *pPlaybackChar = pSoundService->createCharacteristic(
                                            PLAYBACK_CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_WRITE
                                        );

    pServer->setCallbacks(this);
    
    pSegmentChar->setCallbacks(this);
    pShowTimeChar->setCallbacks(this);
    pBrightnessChar->setCallbacks(this);
    pTimeChar->setCallbacks(this);
    pVolumeChar->setCallbacks(this);
    pPlaybackChar->setCallbacks(this);

    pDisplayService->start();
    pTimeService->start();
    pSoundService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(DISPLAY_SERVICE_UUID);
    pAdvertising->addServiceUUID(TIME_SERVICE_UUID);
    pAdvertising->addServiceUUID(SOUND_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    return 0;
}