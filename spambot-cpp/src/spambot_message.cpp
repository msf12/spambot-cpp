#include "spambot_message.h"

uint16_t ParseBadges(string &badges)
{
	uint16_t Result = 0;

	// TODO: simplify ALL of this logic to act like the other functions
	// marker = find(',')
	// substr(index, marker-index) <- double check this works if marker is -1?
	// replace switches with ifs
	for(size_t index = 0, marker = index; index < badges.length(); ++index)
	{
		if(badges[index] == ',' || index == badges.length()-1)
		{
			switch(badges[marker])
			{
				case 'a':
				{
					Result |= 1 << 1;
				} break;
				case 'g':
				{
					Result |= 1 << 2;
				} break;
				case 'm':
				{
					Result |= 1 << 3;
				} break;
				case 's':
				{
					Result |= 1 << 4;
				} break;
				case 't':
				{
					if(badges[marker] == 'w')
					{
						Result |= 1 << 0;
					}
					else
					{
						Result |= 1 << 5;
					}
				} break;
				case 'b':
				{
					if(badges[marker+1] == 'r')
					{
						Result |= 1 << 6;
					}
					else
					{
						marker += 5;
						if(badges[marker] == '5')
						{
							Result |= 1 << 10;
						}
						else
						{
							switch(index - marker)
							{
								case 1:
								{
									Result |= 1 << 7;
								} break;
								case 3:
								{
									Result |= 1 << 8;
								} break;
								case 4:
								{
									Result |= 1 << 9;
								} break;
								case 5:
								{
									Result |= 1 << 11;
								} break;
								case 6:
								{
									Result |= 1 << 12;
								} break;
							}
						}
					}
				} break;
			}
			marker = index+1;
		}
	}
	return Result;
}

TwitchMessage InterpretMessage(string raw_message)
{
	TwitchMessage message = {};

	size_t prefix_start = raw_message.find(' ')+1,
	       prefix_username_end = raw_message.find('!', prefix_start),
	       command_start = raw_message.find(' ', prefix_start)+1,
	       channel_start = raw_message.find(' ', command_start)+1,
	       message_start = raw_message.find(' ', channel_start)+1;

	for(size_t tag_start = 1, tag_middle = 0, tag_end = 0;
		tag_start < prefix_start-1;
		tag_start = tag_end + 1)
	{
		tag_middle = raw_message.find('=',tag_start);
		tag_end = raw_message.find(';',tag_start);
		if(tag_end == -1)
		{
			tag_end = prefix_start-1;
		}

		string tag_name = raw_message.substr(tag_start, tag_middle++-tag_start);
		string tag_value = raw_message.substr(tag_middle, tag_end-tag_middle);

		if(tag_name == "badges")
		{
			message.badges = ParseBadges(tag_value);
		}
		else if(tag_name == "bits")
		{
			message.bits = (uint32_t)SubStringToUInt(tag_value, 0, tag_value.length());
		}
		else if(tag_name == "color")
		{
			message.color.hex = (uint32_t)HexSubStringToUInt(tag_value, 1, tag_value.length());
		}
		else if(tag_name == "display-name")
		{
			message.display_name = tag_value;
		}
		else if(tag_name == "emotes")
		{
			// TODO: Emotes!!!!!
			// message. = (tag_value);
		}
		else if(tag_name == "id")
		{
			message.id = tag_value;
		}
		else if(tag_name == "room-id")
		{
			message.room_id = (uint16_t)SubStringToUInt(tag_value, 0, tag_value.length());
		}
		else if(tag_name == "mod")
		{
			message.mod = tag_value == "1";
		}
		else if(tag_name == "subscriber")
		{
			message.subscriber = tag_value == "1";
		}
		else if(tag_name == "turbo")
		{
			message.turbo = tag_value == "1";
		}
		else if(tag_name == "sent-ts")
		{
			// TODO: sent-ts
			// message. = (tag_value);
		}
		else if(tag_name == "tmi-sent-ts")
		{
			// TODO: tmi-sent-ts
			// message. = (tag_value);
		}
		else if(tag_name == "user-id")
		{
			message.user_id = (uint32_t)SubStringToUInt(tag_value, 0, tag_value.length());
		}
		else if(tag_name == "user-type")
		{
			if(!tag_value.empty())
			{
				switch(tag_value[0])
				{
					case 'a':
					{
						message.user_type = ADMIN;
					} break;
					case 'g':
					{
						message.user_type = GLOBAL_MOD;
					} break;
					case 'm':
					{
						message.user_type = MOD;
					} break;
					case 's':
					{
						message.user_type = STAFF;
					} break;
				}
			}
			else
			{
				message.user_type = EMPTY;
			}
		}
	}

	message.username = raw_message.substr(prefix_start+1, prefix_username_end - (prefix_start+1));

	message.message_type = raw_message.substr(command_start, channel_start-1 - command_start) == "PRIVMSG" ? PRIVMSG : OTHER;

	message.channel = raw_message.substr(channel_start+1, message_start-1 - (channel_start+1));

	message.text = raw_message.substr(message_start+1, (raw_message.length()-2) - (message_start+1));

	return message;
}
