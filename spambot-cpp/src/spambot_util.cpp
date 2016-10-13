inline string FormatTwitchPRIVMSG(string Message)
{
	return "PRIVMSG #" + CHAN + " :" + Message + "\r\n";
}

uint64_t HexSubStringToUInt(string str, size_t start, size_t end)
{
	uint64_t Result = 0;
	for (auto i = start; i < end; ++i)
	{
		Result *= 16;
		if(str[i] < 58)
		{
			Result += str[i] - 48;
		}
		else
		{
			Result += str[i] - 55;
		}
	}
	return Result;
}

uint64_t SubStringToUInt(string str, size_t start, size_t end)
{
	uint64_t Result = 0;
	for (auto i = start; i < end; ++i)
	{
		Result *= 10;
		Result += str[i] - 48;
	}
	return Result;
}

bool SubStringEquals(const char *left, const char *right,size_t start, size_t length)
{
	for (auto i = start; i < start+length; ++i)
	{
		if (left[i] != right[i-start])
		{
			return false;
		}
	}
	return true;
}