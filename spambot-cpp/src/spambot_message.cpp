#include "spambot_message.h"

inline string TimeValueToString(uint64_t time_value)
{
	string Result = to_string(time_value);
	return Result.length() == 1 ? "0" + Result : Result;
}

// Badges bits from lowest to highest order:
// administrator, global_moderator, moderator, subscriber, twitch_staff, turbo, broadcaster, bits1, bits100, bits1000, bits5000, bits10000, bits100000
uint16_t ParseBadges(string &badges)
{
	uint16_t Result = 0;

	for(size_t index = 0, marker; index < badges.length(); index = marker+1)
	{
		marker = badges.find(',', index);
		if(marker == -1)
		{
			marker = badges.length();
		}

		string badge = badges.substr(index, marker-index);

		if(badge == "admin/1")
		{
			Result |= 1 << 1;
		}
		else if(badge == "global_moderator/1")
		{
			Result |= 1 << 2;
		}
		else if(badge == "moderator/1")
		{
			Result |= 1 << 3;
		}
		else if(badge == "subscriber/1")
		{
			Result |= 1 << 4;
		}
		else if(badge == "twitch_staff/1")
		{
			Result |= 1 << 0;
		}
		else if(badge == "turbo/1")
		{
			Result |= 1 << 5;
		}
		else if(badge == "broadcaster/1")
		{
			Result |= 1 << 6;
		}
		else if(badge == "bits/1")
		{
			Result |= 1 << 7;
		}
		else if(badge == "bits/100")
		{
			Result |= 1 << 8;
		}
		else if(badge == "bits/1000")
		{
			Result |= 1 << 9;
		}
		else if(badge == "bits/5000")
		{
			Result |= 1 << 10;
		}
		else if(badge == "bits/10000")
		{
			Result |= 1 << 11;
		}
		else if(badge == "bits/100000")
		{
			Result |= 1 << 12;
		}
	}

	return Result;
}

TwitchMessage InterpretChatMessage(string raw_message)
{
	// TODO: handle Twitch extra commands:
	/*
	@msg-id=slow_on :tmi.twitch.tv NOTICE #usernameop :This room is now in slow mode. You may send messages every 120 seconds.
	@slow=120 :tmi.twitch.tv ROOMSTATE #usernameop
	@msg-id=slow_off :tmi.twitch.tv NOTICE #usernameop :This room is no longer in slow mode.
	@slow=0 :tmi.twitch.tv ROOMSTATE #usernameop

	TO: @ban-duration=1;ban-reason= :tmi.twitch.tv CLEARCHAT #usernameop :dumjethel
	BAN: @ban-reason= :tmi.twitch.tv CLEARCHAT #usernameop :dumjethel
	Unbans aren't notified

	Resubs:
	@badges=staff/1,broadcaster/1,turbo/1;color=#008000;display-name=TWITCH_UserName;emotes=;mod=0;msg-id=resub;msg-param-months=6;room-id=1337;subscriber=1;system-msg=TWITCH_UserName\shas\ssubscribed\sfor\s6\smonths!;login=twitch_username;turbo=1;user-id=1337;user-type=staff :tmi.twitch.tv USERNOTICE #channel :Great stream -- keep it up!
	@badges=staff/1,broadcaster/1,turbo/1;color=#008000;display-name=TWITCH_UserName;emotes=;mod=0;msg-id=resub;msg-param-months=6;room-id=1337;subscriber=1;system-msg=TWITCH_UserName\shas\ssubscribed\sfor\s6\smonths!;login=twitch_username;turbo=1;user-id=1337;user-type=staff :tmi.twitch.tv USERNOTICE #channel
	*/
	TwitchMessage message = {};

	bool IsUserMessage = !(raw_message.find("NOTICE") < 0 &&
	                     raw_message.find("USERNOTICE") < 0 &&
	                     raw_message.find("ROOMSTATE") < 0 &&
	                     raw_message.find("CLEARCHAT") < 0);

	if(IsUserMessage)
	{
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
				auto total_seconds = SubStringToUInt(tag_value,0,tag_value.length());
				total_seconds /= 1000;

				uint8_t years_until_leap_year = 2;
		
				while(total_seconds > SECONDS_PER_DAY)
				{
					if(years_until_leap_year == 0 && total_seconds > SECONDS_PER_LEAP_YEAR)
					{
						total_seconds -= SECONDS_PER_LEAP_YEAR;
						years_until_leap_year = 3;
					}
					else if(years_until_leap_year && total_seconds > SECONDS_PER_NON_LEAP_YEAR)
					{
						total_seconds -= SECONDS_PER_NON_LEAP_YEAR;
						--years_until_leap_year;
					}
					else
					{
						total_seconds -= SECONDS_PER_DAY;
					}
				}

				auto total_minutes = total_seconds/60;
				auto hours = total_minutes/60;
				
				auto minutes = total_minutes - hours * 60;
				auto seconds = total_seconds - total_minutes * 60;

				string hour_string, minute_string, second_string;
				
				hour_string = TimeValueToString(hours);
				minute_string = TimeValueToString(minutes);
				second_string = TimeValueToString(seconds);

				message.sent_ts = hour_string + ":" + minute_string + ":" + second_string;
			}
			else if(tag_name == "tmi-sent-ts")
			{
				auto total_seconds = SubStringToUInt(tag_value,0,tag_value.length());
				total_seconds /= 1000;

				uint8_t years_until_leap_year = 2;
		
				while(total_seconds > SECONDS_PER_DAY)
				{
					if(years_until_leap_year == 0 && total_seconds > SECONDS_PER_LEAP_YEAR)
					{
						total_seconds -= SECONDS_PER_LEAP_YEAR;
						years_until_leap_year = 3;
					}
					else if(years_until_leap_year && total_seconds > SECONDS_PER_NON_LEAP_YEAR)
					{
						total_seconds -= SECONDS_PER_NON_LEAP_YEAR;
						--years_until_leap_year;
					}
					else
					{
						total_seconds -= SECONDS_PER_DAY;
					}
				}

				auto total_minutes = total_seconds/60;
				auto hours = total_minutes/60;
				
				auto minutes = total_minutes - hours * 60;
				auto seconds = total_seconds - total_minutes * 60;

				string hour_string, minute_string, second_string;

				hour_string = to_string(hours);
				if(hour_string.length() == 1)
				{
					hour_string = "0" + hour_string;
				}

				minute_string = to_string(minutes);
				if(minute_string.length() == 1)
				{
					minute_string = "0" + minute_string;
				}

				second_string = to_string(seconds);
				if(second_string.length() == 1)
				{
					second_string = "0" + second_string;
				}

				message.tmi_sent_ts = hour_string + ":" + minute_string + ":" + second_string;
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
	}
	else
	{
		WriteToDebugOut("Message received that was not a user message: " + raw_message);
	}

	return message;
}
