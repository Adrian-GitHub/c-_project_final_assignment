/* SORTING CLASS, USED FOR SORTING STUFF BY FIRST WORD WITHIN STRING */
class Data
{
	friend bool operator<(const Data& lhs, const Data& rhs)
	{
		return lhs.value < rhs.value; //returing sorted bits by value (their first value)
	}

public:
	Data(int x, const std::string& y) : value(x), str(y) {}
	const std::string& getStr()
	{
		return str; //returning sorted string
	}

private:
	int value;
	std::string str;
};