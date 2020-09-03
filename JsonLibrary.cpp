#include "JsonLibrary.h"


namespace  JsonLibrary{

	/*
	Description:	the function fill the unordered_map of the Json through use of string

	Argument:		const wstring& line- the line wich the data is filled from

	Algorithm:		clear white spaces and then verify that the first char is '{' which indicate it is the start of a Json object
					while (didn't reach the end of the string and the input is valid){
						find the key
						pair the value to the key
						check if the string is valid and if another iteration is needed
					}
	*/
	void Json::FillContainer(const wstring& line)
	{
		int i = 0;
		wstring key;
		bool corruptString = false;
		while (iswspace(line[i]) != 0)//skipping white spaces at the start of the file
			i++;
		if (line[i] != '{')//a Json file must start with '{'  
			return;
		i++;
		while (i < (int)line.length() && corruptString==false){
			key = FindKey(line, i, corruptString);
			if (corruptString == true)
				break;
			FindStartOfValue(line, i);
			container[move(key)] = FindValue(line, i, corruptString);
			if (corruptString == true || CheckIfThereIsMorePair(line, i) == false)
				break;
		}
	}

	/*
	Description:	the function find extract the key from the input line
	
	Argument:		const wstring& line	- the input line
					int& index			- the index of the current location in the string
					bool& corruptString	- indicate if the string is valid or not
	
	Algorithm:		clear space
					if (the the key start with \"){
						the key is not valid
					}
					while (the index didnt reach the end of the string){
						if (we reach \"){
							if (before the quotes there is '\'){
								the quotes are part of the key and another iteration is needed
							}
							else{
								this is the end of the key
							}
						}
						add the current char to the key
					}
					point to the char after the end of the key
					return the key

	*/
	wstring Json::FindKey(const wstring& line, int& index, bool& corruptString)
	{
		wstring key = L"";
		while (iswspace(line[index]) != 0)//clearing white spaces
			index++;
		if (line[index] != L'\"') {//a key must be between quotes
			corruptString = true;
			return key;
		}
		index++;
		while (index< (int)line.length()) {
			if (line[index] == L'\"') {
				if (line[index - 1] == L'\\');//the quotes are part of the key
				else
					break;
			}
			key += line[index];
			index++;
		}
		index++;
		return key;
	}

	/*
	Description:	the function find the value of the field
	
	Argument:		const wstring& line	- the input line
					int& index			- the index of the current location in the string
					bool& corruptString	- indicate if the string is valid or not
	
	Algorithm:		construct the new node (deafult constructor is null value)
					handle each case of the value types:
						string
						array
						Json object
						bool or null
						numerical value
						deafult -corrupt value
					return val
	*/

	Json::JsonNode Json::FindValue(const wstring& line, int& index, bool& corruptString)
	{
		JsonNode val;
		if (line[index] == L'\"') {//string
			index++;
			int endOfString = index;
			FindEndOfStringValue(line, endOfString);
			val = line.substr(index, endOfString - index);
			index = endOfString + 1;
			if (index == line.length())
				corruptString = true;
		}
		else if (line[index] == L'[') {//array
			corruptString = CreateArrayValue(line, index, val);
		}
		else if (line[index] == L'{') {//another Json object
			corruptString = CreateObjectValue(line, index, val);
		}
		else if (iswalpha(line[index])) {//bool or null
			corruptString = !CreateBoolOrNull(line, index, val);
		}
		else if (iswdigit(line[index])){//number
			val = CreateNumberValue(line, index, corruptString);
		}
		else {//undefined-null
			corruptString = true;
		}

		return val;
	}

	/*
	Description:	the function find the start of the value field in the input string
	
	Argument:		const wstring& line	- the input line
					int& index			- the index of the current location in the string
	
	*/

	void Json::FindStartOfValue(const wstring& line, int& index)
	{
		while (index < (int)line.length() && line[index] != L':')// find ':' which indicate the start of the value
			index++;
		index++;// point the next character in the string
		while (index < (int)line.length() && (iswspace(line[index]) != 0))
			index++;
	}

	/*
	Description:	the function create float or integer numerical value
	
	Argument:		const wstring& line	- the input line
					int& index			- the index of the current location in the string
					bool& corruptString	- indicate if the string is valid or not
	
	Algorithm:		determine if the value is integer or float number
					if float{
						create float value
					}
					if integer{
						create int value
					}
					else - corrupt input

					advance index to the end of the value

	*/

	Json::JsonNode Json::CreateNumberValue(const wstring& line, int& index, bool& corruptString)
	{
		Json_Field_Type valType;
		int endOfVal;
		JsonNode res;
		valType = DetermineNumericalType(line, index, endOfVal);
		if (valType == Json_Field_Type::FLOAT_FIELD) {
			res = stof(line.substr(index, endOfVal - index));
		}
		else if (valType == Json_Field_Type::INT_FILED) {
			res = stoi(line.substr(index, endOfVal - index));
		}
		else {
			corruptString = true;
		}
		index = endOfVal;
		return res;
	}

	/*
	Description:	the function determine if the value is integer or float number
	
	Argument:		const wstring& line	- the input line
					int& index			- the index of the current location in the string
					int& end			- the index of the end of the value
	
	Algorithm:		deafult type is integer
					while didnt reach the end of the string{
						if the current character is digit continue

						if the character is '.'{

							if a dot didnt appeared before{
								define the type to float
								mark that a dot has occured
							}
							else - a dot has occured before
								corrupt value
						}
						else if the character is white space{
							mark that white space has occured
							break
						}
						else {
							check if the character indicate it is an end of array, end of object or end of value
						}
					}

					double check that the value is valid
	*/

	Json_Field_Type Json::DetermineNumericalType(const wstring& line, const int& start, int& end)
	{
		Json_Field_Type res = Json_Field_Type::INT_FILED;
		bool space_notice = false;
		bool dot_notice = false;
		end = start;
		while (end < (int)line.length()) {
			if (iswdigit(line[end])) {
			}
			else if (line[end] == L'.') {
				if (!dot_notice) {
					res = Json_Field_Type::FLOAT_FIELD;
					dot_notice = true;
				}
				else {
					res = Json_Field_Type::NULL_FILED;
					break;
				}
			}
			else if (iswspace(line[end])) {
				space_notice = true;
				break;
			}
			else {
				if (line[end] != L'}' && line[end] != L',' && line[end] != L']')
					res = Json_Field_Type::NULL_FILED;
				break;
			}
			end++;
		}

		if (space_notice == true && res != Json_Field_Type::NULL_FILED) {
			while (end < (int)line.length()) {
				if (!iswspace(line[end])) {
					if (line[end] != L'}' && line[end] != L',') {
						res = Json_Field_Type::NULL_FILED;
					}
					break;
				}
				end++;
			}
		}
		return res;
	}

	/*
	Description:	the function create array value
	
	Argument:		const wstring& line	- the input line
					int& index			- the index of the current location in the string
					JsonNode& val		- the output value
	
	Algorithm:		//TODO:: end the documintation
	*/

	bool Json::CreateArrayValue(const wstring& line, int& index, JsonNode& val)
	{

		int endOfArray = FindClosureToSegment(line, L'[', index) + 1;
		if (endOfArray == -1)
			return true;
		wstring sub = line.substr(index, endOfArray - index);
		bool corrupString = false;
		index = endOfArray;
		int i = 1;

		val = vector<JsonNode>();//define val as array type json node
		while (i < (int)sub.length()) {
			SkipWhiteSpaces(sub, i);
			((vector<JsonNode>&)(val)).push_back(FindValue(sub, i, corrupString));
			if (corrupString == true)
				return corrupString;
			if (CheckIfThereIsMorePair(sub, i) == false)
				break;
		}
		return false;
	}

	int Json::FindClosureToSegment(const wstring& line, wchar_t c, int index)
	{
		wchar_t closure;
		if (c == L'{') {
			closure = L'}';
		}
		else if (c == L'[') {
			closure = L']';
		}
		else {//unsupport value
			return -1;
		}
		int count = 0;
		while (index < line.length()) {
			if (line[index] == c) {
				count++;
			}
			else if (line[index] == closure) {
				count--;
			}
			if (count == 0)
				return index;
			index++;
		}
		return -1;//the string doesnt contain leagal input
	}

	void Json::FindEndOfStringValue(const wstring& line, int& end)
	{
		while (end < (int)line.length()) {
			if (line[end] == L'\"') {
				if (line[end - 1] == L'\\');
				else
					break;
			}
			end++;
		}
	}

	bool Json::CheckIfThereIsMorePair(const wstring& line, int& index)
	{
		wchar_t t;
		while (index < line.length() && iswspace(line[index])) {
			index++;
			t = line[index];
		}
		if (index < line.length() && line[index++] == L',')
			return true;
		return false;
	}

	void Json::SkipWhiteSpaces(const wstring& line, int& index)
	{
		while (index < line.length() && iswspace(line[index])!=0) {
			index++;
		}
	}

	bool Json::CreateBoolOrNull(const wstring& line, int& index, JsonNode& val)
	{
		wstring temp = L"null";	
		if (line.compare(index, temp.length(), temp) == 0) {
			index += temp.length();
			return true;
		}
		temp = L"true";
		if (line.compare(index, temp.length(), temp) == 0) {
			val = true;
			index += temp.length();
			return true;
		}
		temp = L"false";
		if (line.compare(index, temp.length(), temp) == 0) {
			val = false;
			index += temp.length();
			return true;
		}
		return false;
	}

	bool Json::CreateObjectValue(const wstring& line, int& index, JsonNode& val)
	{
		int endOfArray = FindClosureToSegment(line, L'{', index);
		if (endOfArray == -1)
			return true;

		wstring sub = line.substr(index, endOfArray - index);
		index = endOfArray + 1;
		int i = 0;

		val = make_shared<Json>((Json(sub)));
		return false;
	}

	Json::Json(const wstring& line)
	{
		operator=(line);
	}

	Json::Json(const Json& other)
	{
		container = other.container;
	}

	Json::Json(Json&& other) noexcept
	{
		if (this == &other)
			return;
		container = move(other.container);
	}

	Json::~Json()
	{
	}

	Json Json::operator=(const wstring& line)
	{
		container.clear();
		FillContainer(line);
		return *this;
	}

	Json Json::operator=(const Json& other)
	{
		if (this == &other)
			return *this;
		container.clear();
		container = other.container;
		return *this;
	}
	
	Json::JsonNode::JsonNode(const JsonNode& other)
	{
		if (this == &other)
			return;
		(*this).operator=(other);
	}

	Json::JsonNode::JsonNode(JsonNode&& other) noexcept
	{
		(*this).operator=(move(other));
	}
	
	Json::JsonNode& Json::JsonNode::operator=(const JsonNode& other)
	{
		if (this == &other)
			return *this;
		ClearSpace(other.type);
		type = other.type;
		switch (other.type) {
		case Json_Field_Type::STRING_FILED:
			str = other.str;
			break;
		case Json_Field_Type::INT_FILED:
			Int = other.Int;
			break;
		case Json_Field_Type::FLOAT_FIELD:
			Float = other.Float;
			break;
		case Json_Field_Type::OBJ_FILED:
			jsn = other.jsn;
			break;
		case Json_Field_Type::ARRAY_FIELD:
			vec = other.vec;
			break;
		case Json_Field_Type::BOOL_FILED:
			Bool = other.Bool;
			break;
		case Json_Field_Type::NULL_FILED:
		default:
			break;
		}
		return *this;
		
	}

	Json::JsonNode& Json::JsonNode::operator=(JsonNode&& other) noexcept
	{
		if (this == &other)
			return *this;
		ClearSpace(other.type);
		type = other.type;
		switch (other.type) {
		case(Json_Field_Type::STRING_FILED):
			str = move(other.str);
			break;
		case(Json_Field_Type::INT_FILED):
			Int = other.Int;
			break;
		case(Json_Field_Type::FLOAT_FIELD):
			Float = other.Float;
			break;
		case(Json_Field_Type::OBJ_FILED):
			jsn = move(other.jsn);
			break;
		case(Json_Field_Type::ARRAY_FIELD):
			vec = move(other.vec);
			break;
		case(Json_Field_Type::BOOL_FILED):
			Bool = other.Bool;
			break;
		case(Json_Field_Type::NULL_FILED):
		default:
			break;
		}
		return *this;
	}

	Json::JsonNode& Json::JsonNode::operator=(const wstring& line)
	{
		ClearSpace(Json_Field_Type::STRING_FILED);
		str = line;
		type = Json_Field_Type::STRING_FILED;
		return *this;
	}

	Json::JsonNode& Json::JsonNode::operator=(wstring&& line)
	{
		ClearSpace(Json_Field_Type::STRING_FILED);
		str = move(line);
		type = Json_Field_Type::STRING_FILED;
		return *this;
	}

	Json::JsonNode& Json::JsonNode::operator=(const wchar_t* c)
	{
		ClearSpace(Json_Field_Type::STRING_FILED);
		str = c;
		type = Json_Field_Type::STRING_FILED;
		return *this;
	}

	Json::JsonNode& Json::JsonNode::operator=(const int& num)
	{
		ClearSpace(Json_Field_Type::INT_FILED);
		Int = num;
		type = Json_Field_Type::INT_FILED;
		return *this;
	}

	Json::JsonNode& Json::JsonNode::operator=(const float& f)
	{
		ClearSpace(Json_Field_Type::FLOAT_FIELD);
		Float = f;
		type = Json_Field_Type::FLOAT_FIELD;
		return *this;
	}

	Json::JsonNode& Json::JsonNode::operator=(const shared_ptr<Json>& ptr)
	{
		ClearSpace(Json_Field_Type::OBJ_FILED);
		jsn = ptr;
		type = Json_Field_Type::OBJ_FILED;
		return *this;
	}

	Json::JsonNode& Json::JsonNode::operator=(shared_ptr<Json>&& ptr)
	{
		ClearSpace(Json_Field_Type::OBJ_FILED);
		jsn = move(ptr);
		type = Json_Field_Type::OBJ_FILED;
		return *this;
	}

	Json::JsonNode& Json::JsonNode::operator=(const vector<JsonNode>& v)
	{
		ClearSpace(Json_Field_Type::ARRAY_FIELD);
		vec = v;
		type = Json_Field_Type::ARRAY_FIELD;
		return *this;
	}

	Json::JsonNode& Json::JsonNode::operator=(vector<JsonNode>&& v)
	{
		ClearSpace(Json_Field_Type::ARRAY_FIELD);
		vec = move(v);
		type = Json_Field_Type::ARRAY_FIELD;
		return *this;
	}

	Json::JsonNode& Json::JsonNode::operator=(bool b)
	{
		ClearSpace(Json_Field_Type::BOOL_FILED);
		Bool = b;
		type = Json_Field_Type::BOOL_FILED;
		return *this;
	}

	wstring Json::JsonNode::ArrToString() const
	{
		wstring temp = L"[";
		for (int i = 0; i < vec.size(); i++) {
			temp += vec[i].ConvertToString() + L",";
		}
		if (temp[temp.length() - 1] == L',') {//non-empty array
			temp[temp.length() - 1] = L']';
		}
		else {
			temp += L']';//empty array
		}
		return temp;
	}

	wstring Json::JsonNode::BoolToString() const
	{
		wstring temp = L"false";
		if (Bool == true) {
			temp = L"true";
		}
		return temp;
	}

	void Json::JsonNode::ClearSpace(const Json_Field_Type& newType)
	{
		if (type == Json_Field_Type::ARRAY_FIELD ||
			type == Json_Field_Type::OBJ_FILED ||
			type == Json_Field_Type::STRING_FILED) {
			if (type == newType)
				return;
			if (type == Json_Field_Type::ARRAY_FIELD)
				vec.clear();
			else if (type == Json_Field_Type::OBJ_FILED)
				jsn.reset();
			else {//string field
				str.clear();
			}
		}

	}

	Json::JsonNode& Json::JsonNode::operator[](const int& i)
	{
		if (type != Json_Field_Type::ARRAY_FIELD) {
			vec.resize(1);
		}
		if (i > vec.size())
			vec.resize(i);
		return vec[i];
	}

	const Json::JsonNode& Json::JsonNode::operator[](const int& i) const
	{
		if (type != Json_Field_Type::ARRAY_FIELD) {
			vec.resize(1);
		}
		if (i > vec.size())
			vec.resize(i);
		return vec[i];
	}

	wstring Json::JsonNode::ConvertToString() const
	{
		wstring temp;
		switch (type)
		{
		case JsonLibrary::Json_Field_Type::STRING_FILED:
			temp = L'\"' + str + L'\"';
			break;
		case JsonLibrary::Json_Field_Type::INT_FILED:
			temp = to_wstring(Int);			
			break;
		case JsonLibrary::Json_Field_Type::FLOAT_FIELD:
			temp = to_wstring(Float);
			break;
		case JsonLibrary::Json_Field_Type::OBJ_FILED:
			temp = to_string(*jsn);
			break;
		case JsonLibrary::Json_Field_Type::ARRAY_FIELD:
			temp = ArrToString();
			break;
		case JsonLibrary::Json_Field_Type::BOOL_FILED:
			temp = BoolToString();
			break;
		case JsonLibrary::Json_Field_Type::NULL_FILED:
			temp = L"null";
			break;
		default:
			break;
		}
		return temp;
	}

	wostream& operator<<(wostream& os, const Json_Field_Type& t)
	{
		wstring str;
		switch (t)
		{
		case JsonLibrary::Json_Field_Type::STRING_FILED:
			str = L"String";
			break;
		case JsonLibrary::Json_Field_Type::INT_FILED:
			str = L"Integer";
			break;
		case JsonLibrary::Json_Field_Type::FLOAT_FIELD:
			str = L"Float";
			break;
		case JsonLibrary::Json_Field_Type::OBJ_FILED:
			str = L"Json Object";
			break;
		case JsonLibrary::Json_Field_Type::ARRAY_FIELD:
			str = L"Array";
			break;
		case JsonLibrary::Json_Field_Type::BOOL_FILED:
			str = L"Boolean";
			break;
		case JsonLibrary::Json_Field_Type::NULL_FILED:
			str = L"Null";
			break;
		default:
			break;
		}
		return os << str;
	}

	wstring to_string(const Json& me)
	{
		wstring str;
		str = L'{';

		for (auto it = me.container.begin(); it != me.container.end(); it++) {
			wstring temp;
			str += L'\"' + (it->first) + L'\"';
			str += L':';
			str += (it->second).ConvertToString();
			str += L',';
		}
		if(str[str.length() - 1] == L',')//non-empty object
			str[str.length() - 1] = L'}';
		else {//case of empty object
			str += L'}';
		}
		return str;
	}

}


