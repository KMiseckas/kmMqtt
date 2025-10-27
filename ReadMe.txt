TODO:
- Search all TODOs if in doubt.

- Add remaining receive queue callbacks.
- Add session state packet resubmission on start up of cleanSession = 0;
- Gracefull disconnect (see handleInternalDisconnect)

TODO Next:

OPTIMIZATIONS
- Add SBO to byte buffers and lambda MoveOnlyFunction

UNSUBSCRIBE
- Add Unsubscribe.
- Add Unsubscribe Acknowledge.
- Add unsibscribe pending list similar to subscribe. Read MQTT 5 spec.

MQTT EXAMPLE CLIENT
- Implement ping view.
- Implement more flexible connection options view. Maybe bigger window then smaller after connected.
- Add configs view.
- Add publish view.
- Add subscribe view.
- Add ubsubscribe view.
- Add messages/payload view.
- Add log view.
- Improve connection url (make it easier).

SOCKETS
- Add Websocket support.
- Add SSL/TLS BSD sockets support.
- Make interchangeable.

ENCRYPTION & COMPRESSION
- Built in payload encryption/decryption support?
- Built in payload compressions/decompression support?

MEMORY & OPTIMIZATIONS
- Add custom new define for memory profiling.


IMPROVEMENTS
- Wrapper for `pendingSubscriptions` in MqttConnectionInfo.