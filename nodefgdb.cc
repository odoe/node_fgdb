#include <v8.h>
#include <node.h>
#include <FileGDBAPI.h>

#include <string.h>
#include <iostream>
#include <sstream>

using namespace v8;
using namespace node;
using namespace FileGDBAPI;
using namespace std;

class NodeFGDB: ObjectWrap {
private:
	int m_count;
public:
	std::string inQuery;
	std::wstring cleanQuery;
	static Persistent<FunctionTemplate> s_ct;
	static void Init(Handle<Object> target) {
		HandleScope scope;

		Local<FunctionTemplate> t = FunctionTemplate::New(New);

		s_ct = Persistent<FunctionTemplate>::New(t);
		s_ct->InstanceTemplate()->SetInternalFieldCount(1);
		s_ct->SetClassName(String::NewSymbol("NodeFGDB"));

		NODE_SET_PROTOTYPE_METHOD(s_ct, "query", Query);
		NODE_SET_PROTOTYPE_METHOD(s_ct, "fields", Fields);

		target->Set(String::NewSymbol("NodeFGDB"), s_ct->GetFunction());
	}

	NodeFGDB() :
		m_count(0) {
	}

	~NodeFGDB() {
	}

	// wrap the add-on work with node
	// this happens on new constructor
	static Handle<Value> New(const Arguments& args) {
		HandleScope scope;
		NodeFGDB* ng = new NodeFGDB();
		ng->Wrap(args.This());
		return args.This();
	}

	static Handle<Value> Fields(const Arguments& args) {
		HandleScope scope;
		NodeFGDB* ng = ObjectWrap::Unwrap<NodeFGDB>(args.This());
		//NodeFGDB* ng = ObjectWrap::Unwrap<NodeFGDB>(info.Holder());
		ng->m_count++;
		v8::String::Utf8Value v8str(args[0]);
		fgdbError hr;
		wstring errorText;
		Geodatabase geodatabase;
		Local<String> result;
		// Open the geodatabase.
		if ((hr = OpenGeodatabase(L"data/Querying.gdb", geodatabase)) != S_OK) {
			ErrorInfo::GetErrorDescription(hr, errorText);
			string str(errorText.length(), ' ');
			copy(errorText.begin(), errorText.end(), str.begin());
			result = String::New(str.c_str(), str.size());
			return scope.Close(result);
		}

		// Open the Cities table
		Table table;
		if ((hr = geodatabase.OpenTable(L"Cities", table)) != S_OK) {
			ErrorInfo::GetErrorDescription(hr, errorText);
			string str(errorText.length(), ' ');
			copy(errorText.begin(), errorText.end(), str.begin());
			result = String::New(str.c_str(), str.size());
			//result = String::New(errorText);
			return scope.Close(result);
		}

		FieldInfo fInfo;
		table.GetFieldInformation(fInfo);
		int count = 0;
		fInfo.GetFieldCount(count);
		wstring fieldNames = L"";
		for (int x = 0; x < count; x++) {
			wstring f = L"";
			fInfo.GetFieldName(x, f);
			fieldNames.append(f);
			fieldNames.append(L", ");
		}

		string str(fieldNames.length() - 2, ' ');
		copy(fieldNames.begin(), (fieldNames.end() - 2), str.begin());
		result = String::New(str.c_str(), str.size());

		return scope.Close(result);
	}
	static Handle<Value> Query(const Arguments& args) {
		HandleScope scope;
		NodeFGDB* ng = ObjectWrap::Unwrap<NodeFGDB>(args.This());
		//NodeFGDB* ng = ObjectWrap::Unwrap<NodeFGDB>(info.Holder());
		ng->m_count++;
		v8::String::Utf8Value v8str(args[0]);
		ng->inQuery = *v8str ? *v8str : "<string conversion failed>";
		ng->cleanQuery = wstring(ng->inQuery.length(), L' ');
		copy(ng->inQuery.begin(), ng->inQuery.end(), ng->cleanQuery.begin());

		// add the return result here
		//TESTING
		fgdbError hr;
		wstring errorText;
		Geodatabase geodatabase;
		Local<String> result;
		// Open the geodatabase.
		if ((hr = OpenGeodatabase(L"data/Querying.gdb", geodatabase)) != S_OK) {
			ErrorInfo::GetErrorDescription(hr, errorText);
			string str(errorText.length(), ' ');
			copy(errorText.begin(), errorText.end(), str.begin());
			result = String::New(str.c_str(), str.size());
			return scope.Close(result);
		}

		// Open the Cities table
		Table table;
		if ((hr = geodatabase.OpenTable(L"Cities", table)) != S_OK) {
			ErrorInfo::GetErrorDescription(hr, errorText);
			string str(errorText.length(), ' ');
			copy(errorText.begin(), errorText.end(), str.begin());
			result = String::New(str.c_str(), str.size());
			//result = String::New(errorText);
			return scope.Close(result);
		}
		// perform a query
		EnumRows attrQueryRows;
		//if ((hr = table.Search(L"Shape, CITY_NAME, POP1990", L"TYPE = 'city' AND OBJECTID < 10", true, attrQueryRows)) != S_OK)
		//std::wstring qstr = L"TYPE = 'city' AND CITY_NAME = 'Port Angeles'";
		//std::wstring qstr = L"TYPE = 'city' AND CITY_NAME LIKE '%";
		std::wstring qstr = L"";
		qstr.append(ng->cleanQuery);
		//qstr.append(L"%'");
		if ((hr = table.Search(L"Shape, CITY_NAME, POP1990", qstr, true,
				attrQueryRows)) != S_OK) {
			ErrorInfo::GetErrorDescription(hr, errorText);
			string str(errorText.length(), ' ');
			copy(errorText.begin(), errorText.end(), str.begin());
			result = String::New(str.c_str(), str.size());
			//result = String::New(errorText);
			return scope.Close(result);
		}

		// iterate the returned rows
		Row attrQueryRow;
		wstring wQuery;
		wstring names = L"";
		int32 cityPop;
		PointShapeBuffer geometry;
		Point* point;
		while (attrQueryRows.Next(attrQueryRow) == S_OK) {
			attrQueryRow.GetString(L"CITY_NAME", wQuery);
			names.append(wQuery);
			names.append(L", ");
			attrQueryRow.GetGeometry(geometry);
			geometry.GetPoint(point);
		}
		string str(names.length() - 2, ' ');
		copy(names.begin(), (names.end() - 2), str.begin());
		result = String::New(str.c_str(), str.size());
		return scope.Close(result);
	}
};

Persistent<FunctionTemplate> NodeFGDB::s_ct;

extern "C" {
static void init(Handle<Object> target) {
	NodeFGDB::Init(target);
}

NODE_MODULE(nodefgdb, init);
}

