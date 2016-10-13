union Color
{
	uint32_t hex;
	struct
	{
		uint8_t B,G,R;
	};
};

enum Badge
{
	TWITCH_STAFF,
	ADMINISTRATOR,
	GLOBAL_MODERATOR,
	MODERATOR,
	SUBSCRIBER,
	TURBO,
	BITS1,
	BITS100,
	BITS1000,
	BITS5000,
	BITS10000,
	BITS100000
};

enum UserType
{
	EMPTY,
	MOD,
	GLOBAL_MOD,
	ADMIN,
	STAFF
};

enum MessageType
{
	PRIVMSG,
	OTHER
};

struct Emote
{
	uint16_t start, length, emote_id;
	string emote_name;
};

struct TwitchMessage
{
	// bits100000, bits10000, bits5000, bits1000, bits100, bits1, broadcaster, turbo, subscriber, moderator, global_moderator, administrator, twitch_staff
	uint16_t badges;
	uint32_t bits;
	Color color;
	string display_name;
	Emote *emotes;
	string id;
	uint16_t room_id;
	bool mod, subscriber, turbo;
	// TODO: set-ts, tmi-sent-ts
	uint32_t user_id;
	UserType user_type;
	string username; // TODO: is this redundant?
	MessageType message_type;
	string channel; // TODO: this is redundant and can probably be removed
	string text;
};

// This is the makeup of a twitch PRIVMSG tag string
// @

// badges=staff/1,bits/1000;
// badges is a comma-separated list of chat badges, valid badges are staff, admin, global_mod, moderator, subscriber and turbo.

// bits=100;
// bits, if present, means the user used this amount of cheer/bits. All instances of /(^|\s)cheer\d+(\s|$)/ should be replaced with the appropriate emote: static-cdn.jtvnw.net/bits/THEME/TYPE/COLOR/SIZE, where
// THEME is light or dark
// TYPE is animated or static
// COLOR is red for >= 10000 bits, blue for >= 5000, green for >= 1000, purple for >= 100, or otherwise gray
// SIZE is a digit between 1 and 4.

// color=;
// color is a hexadecimal RGB color code
// Empty if it's never been set.

// display-name=TWITCH_UserNaME;
// display-name is the user's display name, escaped as described in the IRCv3 spec.
// Empty if it's never been set.

// emotes=25:0-4,12-16/1902:6-10;
// emotes contains information to replace text in the message with the emote images and can be empty. The format is as follows:
// emote_id:first_index-last_index,another_first-another_last/another_emote_id:first_index-last_index
// emote_id is the number to use in this URL: http://static-cdn.jtvnw.net/emoticons/v1/:emote_id/:size (size is 1.0, 2.0 or 3.0)
// Emote indexes are simply character indexes. \001ACTION does not count and indexing starts from the first character that is part of the user's "actual message". In the example message, the first Kappa (emote id 25) is from character 0 (K) to character 4 (a), and the other Kappa is from 12 to 16.

// id=b34ccfc7-4977-403a-8a94-33c6bac34fb8;
// id is a unique identifier for a message.

// room-id=1337;
// room-id is the ID of the channel.

// mod=0;
// subscriber=0;
// turbo=1;
// mod, subscriber and turbo are either 0 or 1 depending on whether the user has mod, sub or turbo badge or not.

// user-id=1337;
// user-id is the user's ID.

// user-type=staff
// user-type is either empty, mod, global_mod, admin or staff.
// The broadcaster can have any of these, including empty.

// Message after tags
// :twitch_username!twitch_username@twitch_username.tmi.twitch.tv PRIVMSG #channel :cheer100

TwitchMessage InterpretMessage(string raw_message);