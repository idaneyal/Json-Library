#ifndef JSONLIBRARY_H
#define JSONLIBRARY_H

#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

#include <wctype.h>


using namespace std;

namespace JsonLibrary {

	enum class  Json_Field_Type {
		STRING_FILED,
		INT_FILED,
		FLOAT_FIELD,
		OBJ_FILED,
		ARRAY_FIELD,
		BOOL_FILED,
		NULL_FILED
	};

	wostream& operator<<(wostream& os, const  Json_Field_Type& t);

	class Json {

		//friend class test;
		
		class JsonNode {
		private:

			wstring str;
			int Int=0;
			float Float=0;
			shared_ptr<Json> jsn;
			mutable vector<JsonNode> vec;
			
			bool Bool=false;

			Json_Field_Type type;

			wstring ArrToString() const;
			wstring BoolToString()const;

			void ClearSpace(const Json_Field_Type& newType);

		public:
			JsonNode() :
				Int(0),Float(0),Bool(false),type(Json_Field_Type::NULL_FILED){}

			JsonNode(const JsonNode& other);

			JsonNode(JsonNode&& other) noexcept;

			JsonNode(const wstring& line):
				str(line),type(Json_Field_Type::STRING_FILED){}

			JsonNode(wstring&& line) :
				str(move(line)), type(Json_Field_Type::STRING_FILED) {}

			JsonNode(const wchar_t* c) :
				str(c), type(Json_Field_Type::STRING_FILED) {}

			JsonNode(const int& num) :
				Int(num), type(Json_Field_Type::INT_FILED){}

			JsonNode(const float& f):
				Float(f),type(Json_Field_Type::FLOAT_FIELD){}

			JsonNode(const shared_ptr<Json>& ptr):
				jsn(ptr),type(Json_Field_Type::OBJ_FILED){}

			JsonNode(shared_ptr<Json>&& ptr) :
				jsn(move(ptr)), type(Json_Field_Type::OBJ_FILED) {}

			JsonNode(const vector<JsonNode>& v):
				vec(v),type(Json_Field_Type::ARRAY_FIELD){}

			JsonNode(vector<JsonNode>&& v) :
				vec(move(v)), type(Json_Field_Type::ARRAY_FIELD){}

			JsonNode(const bool& b):
				Bool(b),type(Json_Field_Type::BOOL_FILED){}


			~JsonNode(){}

			JsonNode& operator=(const JsonNode& other);
			JsonNode& operator=(JsonNode&& other) noexcept;
			JsonNode& operator=(const wstring& line);
			JsonNode& operator=(wstring&& line);
			JsonNode& operator=(const wchar_t* c);
			JsonNode& operator=(const int& num);
			JsonNode& operator=(const float& f);
			JsonNode& operator=(const shared_ptr<Json>& ptr);
			JsonNode& operator=(shared_ptr<Json>&& ptr);
			JsonNode& operator=(const vector<JsonNode>& v);
			JsonNode& operator=(vector<JsonNode>&& v);
			JsonNode& operator=(bool b);

			operator const int& () { return Int; }
			operator const float& () { return Float; }
			operator const wstring& () { return str; }
			operator const bool& () { return Bool; }
			operator const vector<JsonNode>& () { return vec; }
			operator vector<JsonNode>& () { return vec; }
			operator const shared_ptr<Json>& () { return jsn; }

			JsonNode& operator[](const int& i);

			const JsonNode& operator[](const int& i)const;

			const Json_Field_Type& GetTypeOfNode() {
				return type;
			}
			wstring ConvertToString() const;
		};
		
		unordered_map<wstring, JsonNode> container;
		
		void FillContainer(const wstring& line);
		wstring FindKey(const wstring& line, int& index, bool& corruptString);
		JsonNode FindValue(const wstring& line, int& index, bool& corruptString);
		void FindStartOfValue(const wstring& line, int& index);

		JsonNode CreateNumberValue(const wstring& line, int& index, bool& corruptString);
		Json_Field_Type DetermineNumericalType(const wstring& line, const int& start, int& end);

		bool CreateArrayValue(const wstring& line, int& index, JsonNode& val);
		int FindClosureToSegment(const wstring& line, wchar_t c, int index);

		void FindEndOfStringValue(const wstring& line, int& end);
		bool CheckIfThereIsMorePair(const wstring& line, int& index);

		void SkipWhiteSpaces(const wstring& line, int& index);
		bool CreateBoolOrNull(const wstring& line, int& index, JsonNode& val);

		bool CreateObjectValue(const wstring& line, int& index, JsonNode& val);


	public:

		Json() {};
		Json(const wstring& line);
		Json(const Json& other);
		Json(Json&& other) noexcept;
		~Json();

		Json operator=(const wstring& line);
		Json operator=(const Json& other);

		JsonNode& operator[](const wstring& line) {
			return container[line];
		}

		bool find(const wstring& key) {
			return (container.find(key)!= container.end());
		}

		Json_Field_Type GetType(const wstring& key) {
			return container[key].GetTypeOfNode();
		}

		void Clear() {
			container.clear();
		}

		void Remove(const wstring& key) {
			container.erase(key);
		}

		size_t Size() {
			return container.size();
		}


		friend wstring to_string(const Json& me);

	};

	

}


#endif // !JSONLIBRARY_H

