#pragma once
using namespace System::Collections::Generic;
using namespace System::Data::OleDb;
using namespace System;

ref class User {
public:
	int Id;
	String^ Fullname;
	String^ Login;
	String^ Password;
	String^ Status;

	User() {}
	User(int Id, String^ Fullname, String^ Login, String^ Password, String^ Status) : 
		Id(Id), Fullname(Fullname), Login(Login), Password(Password), Status(Status){}
};

ref class Users
{
private:
	static User^ selectedUser;
	List<User^>^ data;
	String^ connectionString;
	OleDbConnection^ dbConnection;
	String^ tableName;

public:
	Users();

	bool QuerySelectAll();
	bool QueryInsert(User^);
	bool QueryUpdate(User^);
	bool QueryDelete(int id);

	bool QueryHasLogin(String^ login);
	bool QueryAuthorization(String^ login, String^ password);
	bool QueryRegistration(User^);

	int QueryGetLastId();
	User^ QueryGetById(int id);

	User^ GetSelectedUser() { return selectedUser; };
	void SetSelectedUser(User^ d) { selectedUser = d; }

	String^ GetTableName() { return tableName; }
	void SetTableName(String^ name) { tableName = name; }

	List<User^>^ GetData() { return data; }
	int Count() { return data->Count; }

	User^ operator[](int index);
};

