#include "Users.h"

Users::Users()
{
	selectedUser = gcnew User();
	data = gcnew List<User^>();
	connectionString = "provider=Microsoft.Jet.OLEDB.4.0;Data Source=Database.mdb";
	dbConnection = gcnew OleDbConnection(connectionString);
	tableName = "users";
}

bool Users::QuerySelectAll()
{
	bool done = false;

	data->Clear();
	String^ query = "SELECT * FROM [" + tableName + "]";
	OleDbCommand^ dbComand;
	OleDbDataReader^ dbReader;

	try {
		if (dbConnection->State != System::Data::ConnectionState::Open)
			dbConnection->Open();

		dbComand = gcnew OleDbCommand(query, dbConnection);
		 dbReader = dbComand->ExecuteReader();

		if (dbReader->HasRows) {
			done = true;

			while (dbReader->Read()) {
				data->Add(gcnew User(Convert::ToInt32(dbReader["Id"]), dbReader["Fullname"]->ToString(), dbReader["Login"]->ToString(), 
					dbReader["Password"]->ToString(), dbReader["Status"]->ToString()));
			}
		}
	}
	catch (Exception^ ex) {
		throw ex;
	}
	catch (OleDbException^ ex) {
		throw ex;
	}
	finally {
		if (dbConnection->State != System::Data::ConnectionState::Closed)
			dbConnection->Close();
	}

	return done;
}

bool Users::QueryInsert(User^ d)
{
	bool done = false;

	try {
		d->Id = QueryGetLastId() + 1;

		if (dbConnection->State != System::Data::ConnectionState::Open)
			dbConnection->Open();

		String^ query = "INSERT INTO ["+tableName+"] VALUES(" + d->Id + ", '"+ d->Fullname +"', '" + d->Login + "', '" + d->Password + "', '" + d->Status + "')";
		OleDbCommand^ dbComand = gcnew OleDbCommand(query, dbConnection);
		dbComand->ExecuteNonQuery();

		data->Add(d);
		done = true;
	}
	catch (Exception^ ex) {
		throw ex;
	}
	catch (OleDbException^ ex) {
		throw ex;
	}
	finally {
		if(dbConnection->State != System::Data::ConnectionState::Closed)
			dbConnection->Close();
	}

	return done;
}

bool Users::QueryUpdate(User^ d)
{
	bool done = false;
	try {
		if (dbConnection->State != System::Data::ConnectionState::Open)
			dbConnection->Open();

		String^ query = "UPDATE ["+tableName+"] SET users.Fullname = '" + d->Fullname + "', users.Login = '" + d->Login + "', users.Password ='" + d->Password + "', users.Status = '" + d->Status + "' WHERE id = " + d->Id;
		OleDbCommand^ dbComand = gcnew OleDbCommand(query, dbConnection);
		dbComand->ExecuteNonQuery();

		if (data->Count == 0)
			done = true;

		for (int i = 0; i < data->Count; i++) {
			if (data[i]->Id == d->Id) {
				data[i] = d;
				done = true;
			}
		}
	}
	catch (Exception^ ex) {
		throw ex;
	}
	catch (OleDbException^ ex) {
		throw ex;
	}
	finally {
		if (dbConnection->State != System::Data::ConnectionState::Closed)
			dbConnection->Close();
	}

	return done;
}

bool Users::QueryDelete(int id)
{
	bool done = false;
	try {
		if (dbConnection->State != System::Data::ConnectionState::Open)
			dbConnection->Open();

		String^ query = "DELETE FROM [" + tableName + "] WHERE id = " + id;
		OleDbCommand^ dbComand = gcnew OleDbCommand(query, dbConnection);
		dbComand->ExecuteNonQuery();

		for (int i = 0; i < data->Count; i++) {
			if (data[i]->Id == id) {
				data->RemoveAt(i);
				done = true;
			}
		}
	}
	catch (Exception^ ex) {
		throw ex;
	}
	catch (OleDbException^ ex) {
		throw ex;
	}
	finally {
		if (dbConnection->State != System::Data::ConnectionState::Closed)
			dbConnection->Close();
	}

	return done;
}

bool Users::QueryHasLogin(String^ login)
{
	bool has = false;	

	try {
		if (dbConnection->State != System::Data::ConnectionState::Open)
			dbConnection->Open();

		String^ query = "SELECT * FROM [" + tableName + "] WHERE Login = '" + login + "'";
		OleDbCommand^ dbComand = gcnew OleDbCommand(query, dbConnection);
		OleDbDataReader^ dbReader = dbComand->ExecuteReader();

		if (dbReader->HasRows) {
			has = true;
		}

		dbReader->Close();
	}
	catch (Exception^ ex) {
		throw ex;
	}
	catch (OleDbException^ ex) {
		throw ex;
	}
	finally {
		if (dbConnection->State != System::Data::ConnectionState::Closed)
			dbConnection->Close();
	}

	return has;
}

bool Users::QueryAuthorization(String^ login, String^ password)
{
	bool done = false;

	try {
		if (dbConnection->State != System::Data::ConnectionState::Open)
			dbConnection->Open();

		String^ query = "SELECT * FROM [" + tableName + "] WHERE Login LIKE '" + login + "' AND Password LIKE '" + password + "'";
		OleDbCommand^ dbComand = gcnew OleDbCommand(query, dbConnection);
		OleDbDataReader^  dbReader = dbComand->ExecuteReader();

		if (dbReader->HasRows) {
			done = true;

			while (dbReader->Read()) {
				selectedUser = gcnew User(Convert::ToInt32(dbReader["Id"]), dbReader["Fullname"]->ToString(), dbReader["Login"]->ToString(),
					dbReader["Password"]->ToString(), dbReader["Status"]->ToString());
			}
		}

		dbReader->Close();
	}
	catch (Exception^ ex) {
		throw ex;
	}
	catch (OleDbException^ ex) {
		throw ex;
	}
	finally {
		if (dbConnection->State != System::Data::ConnectionState::Closed)
			dbConnection->Close();
	}

	return done;
}

bool Users::QueryRegistration(User^ d)
{
	bool done = false;

	try {
		if (QueryHasLogin(d->Login)) {
			throw gcnew Exception("Логин занят!");
			return false;
		}

		done = QueryInsert(d);
		selectedUser = data[data->Count - 1];
	}
	catch (Exception^ ex) {
		throw ex;
	}
	catch (OleDbException^ ex) {
		throw ex;
	}

	return done;
}

int Users::QueryGetLastId()
{
	int id = -1;

	try {
		if (dbConnection->State != System::Data::ConnectionState::Open)
			dbConnection->Open();

		String^ query = "SELECT * FROM [" + tableName + "]";
		OleDbCommand^ dbComand = gcnew OleDbCommand(query, dbConnection);
		OleDbDataReader^ dbReader = dbComand->ExecuteReader();

		if (dbReader->HasRows) {
			while (dbReader->Read()) {
				int buf = Convert::ToInt32(dbReader["Id"]);
				if (id < buf)
					id = buf;
			}
		}	

		dbReader->Close();
	}
	catch (Exception^ ex) {
		throw ex;
	}
	catch (OleDbException^ ex) {
		throw ex;
	}
	finally {
		if (dbConnection->State != System::Data::ConnectionState::Closed)
			dbConnection->Close();
	}

	return id;
}

User^ Users::QueryGetById(int id)
{
	User^ d = nullptr;

	String^ query = "SELECT * FROM [" + tableName + "] WHERE id = " + id;
	OleDbCommand^ dbComand;
	OleDbDataReader^ dbReader;

	try {
		if (dbConnection->State != System::Data::ConnectionState::Open)
			dbConnection->Open();

		dbComand = gcnew OleDbCommand(query, dbConnection);
		dbReader = dbComand->ExecuteReader();

		if (dbReader->HasRows) {
			while (dbReader->Read()) {
				d = gcnew User(Convert::ToInt32(dbReader["Id"]), dbReader["Fullname"]->ToString(), dbReader["Login"]->ToString(),
					dbReader["Password"]->ToString(), dbReader["Status"]->ToString());
			}
		}
		else {
			throw gcnew Exception("Данные не найдены!");
		}
	}
	catch (Exception^ ex) {
		throw ex;
	}
	catch (OleDbException^ ex) {
		throw ex;
	}
	finally {
		dbReader->Close();

		if (dbConnection->State != System::Data::ConnectionState::Closed)
			dbConnection->Close();
	}

	return d;
}

User^ Users::operator[](int index) {
	if (index < 0 || index >= data->Count) {
		throw gcnew Exception("Индекс вне диапазона!");
	}

	return data[index];
}
