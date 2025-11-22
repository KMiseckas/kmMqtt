#ifndef INCLUDE_MQTTCLIENT_TEXT_H
#define INCLUDE_MQTTCLIENT_TEXT_H

#define TEXT(id, text) static constexpr const char* const id{text};
#define BTN_LABEL(label, postFix) std::string(label).append("##").append(postFix).c_str()
#define TEXT_FORMAT(text, ...) std::string([&]() { \
    char buffer[1024]; \
    snprintf(buffer, sizeof(buffer), text, __VA_ARGS__); \
    return std::string(buffer); \
}())


namespace text
{
//Tab view
TEXT(add_session, "Press '+' tab to add MQTT session")

//Session View
TEXT(session_connection_control_panel_title, "Connection Controls")
TEXT(session_subscription_topics_panel_title, "Topics")
TEXT(session_messages_panel_title, "Messages")
TEXT(session_publishing_panel_title, "Publish Controls")
TEXT(session_log_output_panel_title, "Log Output")
TEXT(session_subscription_topics_panel_not_connected_msg, "Connect to a broker to manage topics.")
TEXT(session_publishing_panel_not_connected_msg, "Connect to MQTT broker to publish messages")
TEXT(session_messages_panel_not_connected_msg, "Connect to a broker to view messages.")

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

//Topic View
TEXT(topics_none_subscribed_status, "No topics subscribed.")
TEXT(topics_unsubscribe_text, "-")
TEXT(topics_subscribe_text, "+")
TEXT(topics_unsubscribe_confirmation, "Unsubscribe from %s?")
TEXT(topics_subscribe_add_topic, "S")
TEXT(topics_subscribe_cancel_add_topic, "C")
TEXT(topics_subscribe_add_topic_tooltip, "Add Topic to Subscriptions")
TEXT(topics_subscribe_cancel_add_topic_tooltip, "Cancel Adding Topic")
TEXT(topics_advanced_options, "Advanced Options")
TEXT(topics_subscribed, "Subscribed")
TEXT(topics_pending, "Pending")
TEXT(topics_error_str, "Error: %s")

//Publish View - Statistics
TEXT(publish_stats_messages_total, "Messages: %zu")
TEXT(publish_stats_pending, "| %zu Pending")
TEXT(publish_stats_success, "| %zu Success")
TEXT(publish_stats_failed, "| %zu Failed")

//Publish View - Form
TEXT(publish_form_title, "Publish Message")
TEXT(publish_form_topic_label, "Topic")
TEXT(publish_form_topic_hint, "Enter topic name...")
TEXT(publish_form_payload_label, "Payload")
TEXT(publish_form_qos_label, "QoS")
TEXT(publish_form_retain_label, "Retain")
TEXT(publish_form_advanced_button, "Advanced")
TEXT(publish_form_publish_button, "Publish Message")
TEXT(publish_form_clear_all_button, "Clear All")

//Publish View - Advanced Options
TEXT(publish_advanced_title, "Advanced Options")
TEXT(publish_advanced_response_topic_label, "Response Topic")
TEXT(publish_advanced_response_topic_hint, "Optional response topic...")
TEXT(publish_advanced_topic_alias_label, "Topic Alias")
TEXT(publish_advanced_message_expiry_label, "Add Message Expiry")
TEXT(publish_advanced_message_expiry_seconds, "seconds")
TEXT(publish_advanced_payload_format_label, "Payload Format")
TEXT(publish_advanced_correlation_data_label, "Correlation Data")
TEXT(publish_advanced_correlation_data_hint, "Optional correlation data...")

//Publish View - Messages List
TEXT(publish_messages_title, "Published Messages")
TEXT(publish_messages_filter_label, "Filter")
TEXT(publish_messages_no_messages, "No messages published")
TEXT(publish_messages_topic_label, "Topic:")

//Publish View - Message Status Panel
TEXT(publish_status_panel_title, "Message Status")
TEXT(publish_status_panel_show_details, "Show Details")
TEXT(publish_status_panel_recent_label, "Recent:")

//Publish View - Message Status
TEXT(publish_status_pending, "Pending")
TEXT(publish_status_sent, "Sent")
TEXT(publish_status_acknowledged, "OK")
TEXT(publish_status_failed, "Failed")

//Publish View - Message Details
TEXT(publish_details_topic_label, "Topic:")
TEXT(publish_details_payload_label, "Payload (%zu bytes):")
TEXT(publish_details_options_title, "Options:")
TEXT(publish_details_qos, "QoS: %d")
TEXT(publish_details_retain_yes, "Retain: Yes")
TEXT(publish_details_retain_no, "Retain: No")
TEXT(publish_details_format_utf8, "Format: UTF-8")
TEXT(publish_details_format_binary, "Format: Binary")
TEXT(publish_details_response_topic, "Response Topic: %s")
TEXT(publish_details_topic_alias, "Topic Alias: %d")
TEXT(publish_details_message_expiry, "Message Expiry: %d seconds")
TEXT(publish_details_timestamp, "Timestamp: %s")
TEXT(publish_details_error, "Error: %s")

//Publish View - Clear Confirmation
TEXT(publish_clear_confirmation_title, "Clear All Messages##ClearConfirm")
TEXT(publish_clear_confirmation_message1, "Are you sure you want to clear all published messages?")
TEXT(publish_clear_confirmation_message2, "This action cannot be undone.")
TEXT(publish_clear_confirmation_yes_button, "Yes, Clear All")
TEXT(publish_clear_confirmation_cancel_button, "Cancel")

//Publish View - Message Details Window
TEXT(publish_details_window_title, "Message Details##MessageDetails")
TEXT(publish_details_window_close, "Close")

//Publish View - QoS Options
TEXT(publish_qos_0, "QoS 0")
TEXT(publish_qos_1, "QoS 1")
TEXT(publish_qos_2, "QoS 2")

//Publish View - Payload Format Options
TEXT(publish_format_utf8, "UTF-8 Text")
TEXT(publish_format_binary, "Binary Data")

//Publish View - Filter Options
TEXT(publish_filter_all, "All Messages")
TEXT(publish_filter_pending, "Pending")
TEXT(publish_filter_sent, "Sent")
TEXT(publish_filter_acknowledged, "Acknowledged")
TEXT(publish_filter_failed, "Failed")

//Messages View - Main
TEXT(messages_view_title, "MQTT Messages")
TEXT(messages_view_clear_button, "Clear All")
TEXT(messages_view_no_messages, "No messages to display")
TEXT(messages_view_filter_label, "Filter:")
TEXT(messages_view_topic_filter_hint, "Filter by topic...")

//Messages View - Columns
TEXT(messages_column_received, "Received")
TEXT(messages_column_sent, "Sent")

//Messages View - Filters
TEXT(messages_filter_all, "All Messages")
TEXT(messages_filter_received, "Received Only")
TEXT(messages_filter_sent, "Sent Only")
TEXT(messages_filter_by_topic, "By Topic")

//Messages View - Message Types
TEXT(messages_type_received, "Received")
TEXT(messages_type_sent, "Sent")

//Messages View - Details Modal
TEXT(messages_details_title, "Message Details##MessageDetails")
TEXT(messages_details_close_button, "Close")
TEXT(messages_details_type_label, "Type:")
TEXT(messages_details_topic_label, "Topic:")
TEXT(messages_details_timestamp_label, "Timestamp:")
TEXT(messages_details_payload_label, "Payload (%zu bytes):")
TEXT(messages_details_qos_label, "QoS:")
TEXT(messages_details_retain_label, "Retain:")
TEXT(messages_details_status_label, "Status:")

//Messages View - Clear Confirmation
TEXT(messages_clear_confirmation_title, "Clear All Messages##MessagesClearConfirm")
TEXT(messages_clear_confirmation_message1, "Are you sure you want to clear all messages?")
TEXT(messages_clear_confirmation_message2, "This action cannot be undone.")
TEXT(messages_clear_confirmation_yes_button, "Yes, Clear All")
TEXT(messages_clear_confirmation_cancel_button, "Cancel")

//Messages View - Status Text
TEXT(messages_status_received, "Received")
TEXT(messages_status_sent_pending, "Pending")
TEXT(messages_status_sent_acknowledged, "Sent")
TEXT(messages_status_sent_failed, "Failed")

//Output View - Main
TEXT(output_copy_all_button, "Copy All")
TEXT(output_clear_button, "Clear Logs")
TEXT(output_no_logs, "No logs to display")

//Output View - Clear Confirmation
TEXT(output_clear_confirmation_title, "Clear All Logs##OutputClearConfirm")
TEXT(output_clear_confirmation_message1, "Are you sure you want to clear all logs?")
TEXT(output_clear_confirmation_message2, "This action cannot be undone.")
TEXT(output_clear_confirmation_yes_button, "Yes, Clear All")
TEXT(output_clear_confirmation_cancel_button, "Cancel")
}

#endif //INCLUDE_MQTTCLIENT_TEXT_H