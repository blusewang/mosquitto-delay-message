# mosquitto-delay-message
A mosquitto plugin to support delay message

You can publish to topic `delay/${seconds}/${target_topic}` to send a delay message.

Tips:
- The max delay seconds is **600**. if delay seconds set `0`, will be ignored.
- The max delay seconds config reference `mosquitto.conf` file where in this project.
- The republished message will lose MQTT5 protocol's properties data.

