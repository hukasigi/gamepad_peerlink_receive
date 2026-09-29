#include "message.h"
#include "peer_link.h"
#include <Arduino.h>

GamepadData received_gamepad_data{};

portMUX_TYPE gamepad_data_mux = portMUX_INITIALIZER_UNLOCKED;

constexpr uint8_t WIFI_CHANNEL = 14;
const peer_id_t   FROM_PEER_ID = 0x11;

void peer_link_recv_cb(const peer_id_t peer_id, const std::vector<Message>& messages) {
    for (const Message& message : messages) {
        if (message.type == static_cast<uint8_t>(MessageType::Gamepad)) {
            if (message.data.size() != sizeof(GamepadData)) {
                Serial.println("invalid gamepad data");
                continue;
            }

            portENTER_CRITICAL(&gamepad_data_mux);

            memcpy(&received_gamepad_data, message.data.data(), sizeof(GamepadData));

            portEXIT_CRITICAL(&gamepad_data_mux);
        }
    }
}

void setup() {
    Serial.begin(115200);
    peer_link_task_init(WIFI_CHANNEL, FROM_PEER_ID);
}

void loop() {
    GamepadData gamepad_data{};

    portENTER_CRITICAL(&gamepad_data_mux);

    memcpy(&gamepad_data, &received_gamepad_data, sizeof(GamepadData));

    portEXIT_CRITICAL(&gamepad_data_mux);

    int8_t l_x = gamepad_data.joystick_left.x;
    Serial.printf("%d\r\n", l_x);

    delay(50);
}
