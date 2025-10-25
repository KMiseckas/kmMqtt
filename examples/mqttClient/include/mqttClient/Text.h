#ifndef INCLUDE_MQTTCLIENT_TEXT_H
#define INCLUDE_MQTTCLIENT_TEXT_H

#define TEXT(id, text) static constexpr const char* const id{text};
#define BTN_LABEL(label, postFix) std::string(label).append("##").append(postFix).c_str()

namespace text
{
//Tab view
TEXT(add_session, "Press '+' tab to add MQTT session")

//Session View
TEXT(session_connection_control_panel_title, "Connection Controls")
TEXT(session_subscription_topics_panel_title, "Topics")
TEXT(session_messages_panel_title, "Messages")
TEXT(session_publishing_panel_title, "Publish Controls")

//Session - Connection Panel
TEXT(session_connection_configs_button_label, "Edit Configuration")
TEXT(session_connection_connect_button_label, "Connect")
TEXT(session_connection_disconnect_button_label, "Disconnect")
TEXT(session_connection_cancel_button_label, "Cancel")

TEXT(session_connection_status_connected, "Connected")
TEXT(session_connection_status_disconnected, "Disconnected")
TEXT(session_connection_status_reconnecting, "Reconnecting")
TEXT(session_connection_status_connecting, "Connecting")
TEXT(session_connection_status_failed, "Failed To Connect")

TEXT(session_connection_status, "Status:")

//Connect address
TEXT(session_connect_address_input, "Address")
TEXT(session_connect_address_input_hint, "Enter IP6, IP4, or URL address for mqtt broker.")

//Basic connect settings
TEXT(session_connect_username, "Username")
TEXT(session_connect_password, "Password")
TEXT(session_connect_clientid, "Client ID")
TEXT(session_connect_cleanstart, "Clean Start?")
TEXT(session_connect_show_advanced_options, "Show Advanced Options")
TEXT(session_connect_hide_advanced_options, "Hide Advanced Options")
TEXT(session_connect_advanced_text_title, "Advanced Options")
TEXT(session_mqtt_tick_async, "Tick Async?")

//Advanced connect settings
TEXT(session_connect_adv_keepalivesec, "Keep Alive in Sec")
TEXT(session_connect_adv_session_expiry_interval, "Session Expiry Interval")
TEXT(session_connect_adv_maximum_receive, "Receive Maximum")
TEXT(session_connect_adv_maximum_topic_aliases, "Max Topic Aliases")
TEXT(session_connect_adv_request_response_info, "Request Response Info?")
TEXT(session_connect_adv_request_problem_info, "Request Problem Info?")
TEXT(session_connect_adv_extended_auth_method, "Extended Auth Method")
TEXT(session_connect_adv_extended_auth_data, "Extended Auth Data")
TEXT(session_connect_adv_will, "Send Will?")
TEXT(session_connect_adv_show_will_options, "Show Will Options")
TEXT(session_connect_adv_hide_will_options, "Show Will Options")

//Will connect settings
TEXT(session_connect_will_options_text_title, "Will Options")
TEXT(session_connect_will_QOS, "Will QoS")
TEXT(session_connect_will_retain_will_msg, "Retain Will Message?")
TEXT(session_connect_will_delay_interval, "Delay Interval")
TEXT(session_connect_will_msg_expiry_interval, "Message Expiry Interval")
TEXT(session_connect_will_content_type, "Content Type")
TEXT(session_connect_will_response_topic, "Response Topic")
TEXT(session_connect_will_correlation_data, "Correlation Data")
TEXT(session_connect_will_topic, "Will Topic")
TEXT(session_connect_will_payload_format, "Payload Format")
TEXT(session_connect_will_payload, "Payload")
}

#endif //INCLUDE_MQTTCLIENT_TEXT_H