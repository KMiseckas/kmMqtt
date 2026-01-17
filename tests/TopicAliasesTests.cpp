#include <doctest.h>
#include <kmMqtt/Mqtt/TopicAliases.h>
#include <cstring>

TEST_SUITE("Topic Aliases")
{
	using kmMqtt::mqtt::TopicAliases;

	TEST_CASE("Add and find topic alias")
	{
		TopicAliases aliases;
		const char* topic1 = "sensor/temperature";
		const char* topic2 = "sensor/humidity";
		const char* out = nullptr;

		CHECK(aliases.tryAddTopicAlias(topic1, 1));
		CHECK(aliases.tryFindTopicName(1, out));
		CHECK(out != nullptr);
		CHECK(std::strcmp(out, topic1) == 0);

		CHECK(aliases.tryAddTopicAlias(topic2, 2));
		CHECK(aliases.tryFindTopicName(2, out));
		CHECK(out != nullptr);
		CHECK(std::strcmp(out, topic2) == 0);
	}

	TEST_CASE("Overwrite topic alias with new topic")
	{
		TopicAliases aliases;
		const char* topic1 = "a";
		const char* topic2 = "b";
		const char* out = nullptr;

		CHECK(aliases.tryAddTopicAlias(topic1, 5));
		CHECK(aliases.tryFindTopicName(5, out));
		CHECK(std::strcmp(out, topic1) == 0);

		// Overwrite alias 5 with a new topic
		CHECK(aliases.tryAddTopicAlias(topic2, 5));
		CHECK(aliases.tryFindTopicName(5, out));
		CHECK(std::strcmp(out, topic2) == 0);
	}

	TEST_CASE("Add same topic/alias again")
	{
		TopicAliases aliases;
		const char* topic = "repeat";
		const char* out = nullptr;

		CHECK(aliases.tryAddTopicAlias(topic, 7));
		CHECK(aliases.tryAddTopicAlias(topic, 7)); // Should return true, no change
		CHECK(aliases.tryFindTopicName(7, out));
		CHECK(std::strcmp(out, topic) == 0);
	}

	TEST_CASE("Invalid topic name")
	{
		TopicAliases aliases;
		const char* out = nullptr;

		CHECK_FALSE(aliases.tryAddTopicAlias(nullptr, 1));
		CHECK_FALSE(aliases.tryAddTopicAlias("", 1));
		CHECK_FALSE(aliases.tryFindTopicName(1, out));
	}

	TEST_CASE("Find non-existent alias")
	{
		TopicAliases aliases;
		const char* out = nullptr;
		CHECK_FALSE(aliases.tryFindTopicName(42, out));
		CHECK(out == nullptr);
	}
}