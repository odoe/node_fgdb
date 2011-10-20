#include <v8.h>
#include <node.h>
#include <stdlib.h>
#include <FileGDBAPI.h>
#include <string.h>
#include <stdio.h>
#include <sstream>

using namespace v8;
using namespace node;
using namespace FileGDBAPI;
using namespace std;

std::wstring gdbConnection = L"";

namespace {

	struct FGDB: ObjectWrap {
		static Handle<Value> New(const Arguments &args);
		//static Handle<Value> Connect(const Arguments &args);
		static Handle<Value> AllFields(const Arguments &args);	
		static Handle<Value> Query(const Arguments &args);
	};

	Handle<Value> FGDB::New(const Arguments &args)
	{
		HandleScope scope;

		assert(args.IsConstructCall());
		FGDB *self = new FGDB();
		self->Wrap(args.This());

		if ( args.Length() == 0 || !args[0]->IsString() ) {
			return ThrowException( Exception::Error( 
				String::New( "Must provie File Geodatabase Name and Path" ) ) );
		} else {
			v8::String::Utf8Value v8str(args[0]);
			std::string tmp = *v8str ? *v8str : "<string conversion failed>";
			gdbConnection = std::wstring(tmp.length(), L' ');
			copy( tmp.begin(), tmp.end(), gdbConnection.begin() );
		}

		return scope.Close(args.This());
	}

	Handle<Value> FGDB::AllFields(const Arguments &args)
	{
		HandleScope scope;

		Local<Value> emit_v = args.This()->Get(String::NewSymbol("emit"));
		assert(emit_v->IsFunction());
		Local<Function> emit_f = emit_v.As<Function>();

		// extract the table name
		v8::String::Utf8Value v8str(args[0]);
		std::string s_table = *v8str ? *v8str : "<string conversion failed>";
		std::wstring table_name = std::wstring(s_table.length(), L' ');
		copy(s_table.begin(), s_table.end(), table_name.begin());

		fgdbError hr;
		std::wstring errorText;
		Geodatabase geodatabase;
		Local<String> result;

		// open the geodatabase
		if ((hr = OpenGeodatabase(gdbConnection, geodatabase)) != S_OK) {
			ErrorInfo::GetErrorDescription(hr, errorText);
			std::string str(errorText.length(), ' ');
			copy(errorText.begin(), errorText.end(), str.begin());
			result = String::New(str.c_str(), str.size());
			Handle<Value> argv[2] = {
				String::New("onAllFields"),
				result
			};
			return scope.Close(result);
		}

		// Open the table
		Table table;
		if ((hr = geodatabase.OpenTable(table_name, table)) != S_OK) {
			ErrorInfo::GetErrorDescription(hr, errorText);
			string str(errorText.length(), ' ');
			copy(errorText.begin(), errorText.end(), str.begin());
			result = String::New(str.c_str(), str.size());
			return scope.Close(result);
		}

		FieldInfo fInfo;
		table.GetFieldInformation(fInfo);
		int count = 0;
		fInfo.GetFieldCount(count);

		Local<Array> results = Array::New( count );
		//int row_index;
		for (int x = 0; x < count; x++) {
			wstring f = L"";
			fInfo.GetFieldName(x, f);
			std::string str(f.length(), ' ');
			copy( f.begin(), f.end(), str.begin() );
			results->Set( Integer::New( x ), String::New( str.c_str() ) );
		}

		geodatabase.CloseTable(table);

		Handle<Value> argv[2] = {
			String::New("onAllFields"),
			results
		};

		TryCatch tc;

		emit_f->Call(args.This(), 2, argv);

		if (tc.HasCaught()) {
			FatalException(tc);
		}

		return Undefined();
	}

	Handle<Value> FGDB::Query(const Arguments &args)
	{
		HandleScope scope;

		Local<Value> emit_v = args.This()->Get(String::NewSymbol("emit"));
		assert(emit_v->IsFunction());
		Local<Function> emit_f = emit_v.As<Function>();

		// extract the table name
		v8::String::Utf8Value v8str(args[0]);
		v8::String::Utf8Value v8str2(args[1]);

		std::string s_table = *v8str ? *v8str : "<string conversion failed>";
		std::string s_query = *v8str2 ? *v8str2 : "<string conversion failed>";

		std::wstring table_name = std::wstring(s_table.length(), L' ');
		copy(s_table.begin(), s_table.end(), table_name.begin());

		std::wstring query = std::wstring(s_query.length(), L' ');
		copy(s_query.begin(), s_query.end(), query.begin());

		fgdbError hr;
		std::wstring errorText;
		Geodatabase geodatabase;
		Local<String> result;

		// open the geodatabase
		if ((hr = OpenGeodatabase(gdbConnection, geodatabase)) != S_OK) {
			ErrorInfo::GetErrorDescription(hr, errorText);
			std::string str(errorText.length(), ' ');
			copy(errorText.begin(), errorText.end(), str.begin());
			result = String::New(str.c_str(), str.size());
			Handle<Value> argv[2] = {
				String::New("onQueryComplete"),
				result
			};
			return scope.Close(result);
		}

		// Open the table
		Table table;
		if ((hr = geodatabase.OpenTable(table_name, table)) != S_OK) {
			ErrorInfo::GetErrorDescription(hr, errorText);
			std::string str(errorText.length(), ' ');
			copy(errorText.begin(), errorText.end(), str.begin());
			result = String::New(str.c_str(), str.size());
			return scope.Close(result);
		}

		// perform a query
		EnumRows attrQueryRows;
		//qstr.append(L"%'");
		if ((hr = table.Search(L"Shape, CITY_NAME", query, true,
				attrQueryRows)) != S_OK) {
			ErrorInfo::GetErrorDescription(hr, errorText);
			std::string str(errorText.length(), ' ');
			copy(errorText.begin(), errorText.end(), str.begin());
			result = String::New(str.c_str(), str.size());
			//result = String::New(errorText);
			return scope.Close(result);
		}

		// iterate the returned rows
		Row attrQueryRow;
		wstring wQuery;
		wstring names = L"";
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
		//Local<Array> results = Array::New( count );

		//int row_index;
		/*
		for (int x = 0; x < count; x++) {
			wstring f = L"";
			fInfo.GetFieldName(x, f);
			std::string str(f.length(), ' ');
			copy( f.begin(), f.end(), str.begin() );
			results->Set( Integer::New( x ), String::New( str.c_str() ) );
		}
		*/

		geodatabase.CloseTable(table);

		Handle<Value> argv[2] = {
			String::New("onQueryComplete"),
			result
		};

		TryCatch tc;

		emit_f->Call(args.This(), 2, argv);

		if (tc.HasCaught()) {
			FatalException(tc);
		}

		return Undefined();
	}

	extern "C" void init(Handle<Object> target)
	{
		HandleScope scope;

		Local<FunctionTemplate> t = FunctionTemplate::New(FGDB::New);
		t->InstanceTemplate()->SetInternalFieldCount(1);
		t->SetClassName(String::New("FGDB"));
		NODE_SET_PROTOTYPE_METHOD(t, "allFields", FGDB::AllFields);
		NODE_SET_PROTOTYPE_METHOD(t, "query", FGDB::Query);

		target->Set(String::NewSymbol("FGDB"), t->GetFunction());
	}
}