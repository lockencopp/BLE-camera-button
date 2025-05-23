
/**
 * @file BLE-Shutter-Button.cpp
 * @brief This file implements a BLE client for connecting to a BLE HID device (e.g., a BLE keyboard).
 *        It scans for a specific BLE device, establishes a connection, and handles notifications for button states.
 *
 * @details
 * - The program uses the ESP32 BLE library to interact with BLE devices.
 * - It scans for a target BLE device with a specific MAC address and name.
 * - Upon finding the device, it connects and subscribes to the HID input characteristic to receive notifications.
 * - The program also handles BLE security, pairing, and reconnection logic.
 * - Button states are processed based on the received notification data.
 * - Shutter, focus, and hold actions are distinguished based on the button hold time.
 *
 * @author Christoph Deussen
 * @date 2025-05-04
 *
 * @dependencies
 * - Arduino.h
 * - BLEDevice.h
 * - BLEUtils.h
 * - BLEScan.h
 * - BLEAdvertisedDevice.h
 * - BLEClient.h
 * - BLESecurity.h
 * - button_handling.h
 *
 * @note
 * - Replace `targetMACAddress` with the MAC address of your target BLE device.
 * - Debug output can be enabled or disabled by setting the `DEBUG` macro.
 *
 * @global_variables
 * - `myDevice`: Pointer to the discovered BLE device.
 * - `keyboardInputCharacteristic`: Pointer to the HID input characteristic.
 * - `doConnect`: Flag to indicate whether to initiate a connection.
 * - `connected`: Flag to indicate whether the device is connected.
 * - `retryScan`: Flag to indicate whether to retry scanning.
 * - `lastScanTime`: Timestamp of the last BLE scan.
 * - `scheduler_tick_now`: Current scheduler tick.
 * - `scheduler_tick_last`: Scheduler tick for timing.
 * - `targetMACAddress`: MAC address of the target BLE device.
 *
 * @functions
 * - `handleNotification(uint8_t *pData, size_t length)`: Processes notification data from the BLE device.
 * - `notifyCallback(BLERemoteCharacteristic *pCharacteristic, uint8_t *pData, size_t length, bool isNotify)`: Callback for BLE notifications.
 * - `MyClientCallback`: Custom BLE client callback class for handling connection and disconnection events.
 * - `MyAdvertisedDeviceCallbacks`: Custom BLE advertised device callback class for handling scan results.
 * - `MySecurity`: Custom BLE security callback class for handling pairing and authentication.
 * - `setupBLEScan()`: Configures and starts the BLE scan.
 * - `connectToDevice()`: Connects to the target BLE device and subscribes to the HID input characteristic.
 * - `setup()`: Initializes the BLE environment and starts scanning.
 * - `loop()`: Main program loop for handling BLE connection, scanning, and button state processing.
 */
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>
#include <BLESecurity.h>

#include "button_handling.h" // Include the button handling header

#define DEBUG false // Enable or disable debug output

// Global variables
static BLEAdvertisedDevice *myDevice = nullptr;                        // Pointer to the discovered BLE device
static BLERemoteCharacteristic *keyboardInputCharacteristic = nullptr; // Pointer to the HID input characteristic
static bool doConnect = false;                                         // Flag to indicate whether to initiate a connection
static bool connected = false;                                         // Flag to indicate whether the device is connected
static bool retryScan = false;                                         // Flag to indicate whether to retry scanning
static unsigned long lastScanTime = 0;                                 // Timestamp of the last BLE scan

String targetMACAddress = "2a:07:98:00:27:02"; // Replace with your target BLE device's MAC address

uint32_t scheduler_tick_now = 0;  // Current scheduler tick
uint32_t scheduler_tick_last = 0; // Scheduler tick for timing

// Function to handle notifications from the BLE device
void handleNotification(uint8_t *pData, size_t length)
{
    uint32_t time_diff = 0;

    button_state_now = pData[0]; // Get the button state from the notification data
    button_time_now = millis();  // Get the current time
}

// Callback function for BLE notifications
void notifyCallback(
    BLERemoteCharacteristic *pCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
#if DEBUG
    // Print the notification data in debug mode
    Serial.print("Notification: ");
    for (size_t i = 0; i < length; i++)
    {
        Serial.printf("%02X ", pData[i]);
    }
    Serial.println();
#endif
    // Process the notification data
    handleNotification(pData, length);
}

// Custom BLE client callback class for handling connection and disconnection events
class MyClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pclient) override
    {
        // Handle actions on connection (if needed)
    }

    void onDisconnect(BLEClient *pclient) override
    {
        // Handle actions on disconnection
        connected = false;
        Serial.println("Disconnected");
    }
};

// Custom BLE advertised device callback class for handling scan results
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice) override
    {
        // Check if the discovered device matches the target device
        if (advertisedDevice.getName() == "BT1818" &&
            advertisedDevice.getAddress().toString() == targetMACAddress.c_str())
        {
            Serial.println("BT1818 found, connecting...");
            myDevice = new BLEAdvertisedDevice(advertisedDevice); // Save the discovered device
            doConnect = true;                                     // Set the flag to initiate connection
            BLEDevice::getScan()->stop();                         // Stop the BLE scan
            retryScan = false;                                    // Reset the retry flag since the device was found
        }
        else if (!retryScan)
        { // Ensure retryScan is set only once
            Serial.println("BT1818 not found, will retry...");
            retryScan = true;             // Set the retry flag
            BLEDevice::getScan()->stop(); // Stop the current scan
        }
    }
};

// Custom BLE security callback class for handling pairing and authentication
class MySecurity : public BLESecurityCallbacks
{
    uint32_t onPassKeyRequest() override
    {
        Serial.println("PassKey Request received.");
        return 0; // Return a default passkey
    }

    void onPassKeyNotify(uint32_t pass_key) override
    {
        Serial.printf("PassKey Notification: %d\n", pass_key);
    }

    bool onConfirmPIN(uint32_t pin) override
    {
        Serial.printf("Confirm PIN: %d\n", pin);
        return true; // Confirm the PIN
    }

    bool onSecurityRequest() override
    {
        Serial.println("Received Security Request.");
        return true; // Accept the security request
    }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) override
    {
        if (cmpl.success)
        {
            Serial.println("✅ Pairing successful.");
        }
        else
        {
            Serial.println("❌ Pairing failed.");
        }
    }
};

// Function to configure and start the BLE scan
void setupBLEScan()
{
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); // Set the callback for scan results
    pBLEScan->setInterval(100);                                                // Set the scan interval
    pBLEScan->setWindow(99);                                                   // Set the scan window
    pBLEScan->start(5, false);                                                 // Start scanning for 5 seconds
    lastScanTime = millis();                                                   // Update the last scan time
}

// Function to connect to the target BLE device
void connectToDevice()
{
    BLEClient *pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback()); // Set the client callback

    Serial.println("Establishing Connection...");
    if (!pClient->connect(myDevice))
    {
        Serial.println("Connection failed!");
        return;
    }

    Serial.println("Connected. Searching HID Service...");
    // Get the HID service
    BLERemoteService *hidService = pClient->getService(BLEUUID((uint16_t)0x1812)); // HID
    if (!hidService)
    {
        Serial.println("❌ HID-Service not found!");
        return;
    }

    // Get the HID input characteristic
    keyboardInputCharacteristic = hidService->getCharacteristic(BLEUUID((uint16_t)0x2a4d));
    if (keyboardInputCharacteristic)
    {
        keyboardInputCharacteristic->registerForNotify(notifyCallback, true, true); // Register for notifications
        connected = true;
        Serial.println("✅ Connection established. Press any Key...");
    }

    // Get the battery service (optional)
    BLERemoteService *batService = pClient->getService(BLEUUID((uint16_t)0x180F));
    if (batService)
    {
        BLERemoteCharacteristic *batChar = batService->getCharacteristic(BLEUUID((uint16_t)0x2A19));
        if (batChar)
        {
            Serial.println("Battery:");
            Serial.println(batChar->readUInt8()); // Read and print the battery level
        }
    }
}

// Setup function to initialize the BLE environment and start scanning
void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE-Keyboard Connection...");

    setup_outputs(); // Set up output pins for button handling

    BLEDevice::init("");                                // Initialize the BLE device
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT); // Set encryption level
    BLEDevice::setSecurityCallbacks(new MySecurity());  // Set security callbacks

    setupBLEScan(); // Start the BLE scan
}

// Main program loop
void loop()
{
    // Check if the scheduler tick has changed
    scheduler_tick_now = millis();
    if (scheduler_tick_last != scheduler_tick_now)
    {

        // Retry the scan if the retry flag is set and enough time has passed
        if (retryScan && ((millis() - lastScanTime) > 5000))
        { // Retry every 5 seconds
            Serial.println("Retrying BLE scan...");
            setupBLEScan();
        }
        else
        {
            // Handle the button state based on the notification data
            handleButtonState();

            if (scheduler_tick_now % 10 == 0)
            {
            }
            if (scheduler_tick_now % 1000 == 0)
            {
                if (doConnect && !connected)
                {
                    // Attempt to connect to the device if the flag is set
                    connectToDevice();
                }
            }
        }
        scheduler_tick_last = millis(); // Update the last tick time
    }
}
