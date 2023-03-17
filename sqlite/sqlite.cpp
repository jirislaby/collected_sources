#include <cstdlib>
#include <iostream>

#include <sqlite3.h>

class SQLHolder {
	sqlite3 *sql;
	SQLHolder() = delete;
public:
	SQLHolder(sqlite3 *sql) : sql(sql) {}
	~SQLHolder() { sqlite3_close(sql); }

	operator sqlite3 *() const { return sql; }
};

class SQLStmtHolder {
	sqlite3_stmt *stmt;
	SQLStmtHolder() = delete;
public:
	SQLStmtHolder(sqlite3_stmt *stmt) : stmt(stmt) {}
	~SQLStmtHolder() { sqlite3_finalize(stmt); }

	operator sqlite3_stmt *() const { return stmt; }
};

int main()
{
	sqlite3 *sql;
	sqlite3_stmt *stmt;
	int ret;

	srand(time(NULL) | (long)&sql);

	ret = sqlite3_open_v2("db.db", &sql, SQLITE_OPEN_READWRITE, NULL);
	SQLHolder sql_holder(sql);
	if (ret != SQLITE_OK) {
		std::cerr << "db open failed: " << sqlite3_errstr(ret) << "\n";
		return EXIT_FAILURE;
	}

	ret = sqlite3_prepare_v2(sql, "SELECT * FROM pokus;", -1, &stmt, NULL);
	SQLStmtHolder sel(stmt);
	if (ret != SQLITE_OK) {
		std::cerr << "db prepare failed: " << sqlite3_errstr(ret) <<
			" -> " << sqlite3_errmsg(sql_holder) << "\n";
		return EXIT_FAILURE;
	}

	while ((ret = sqlite3_step(sel)) == SQLITE_ROW) {
		std::cout << "id=" << sqlite3_column_int(sel, 0) <<
			" i=" << sqlite3_column_int(sel, 1) <<
			" t=" << sqlite3_column_text(sel, 2) << "\n";
	}

	if (ret != SQLITE_DONE) {
		std::cerr << "db step sel failed: " << sqlite3_errstr(ret) <<
			" -> " << sqlite3_errmsg(sql_holder) << "\n";
		return EXIT_FAILURE;
	}

	ret = sqlite3_prepare_v2(sql, "INSERT INTO pokus(val, mtext) VALUES (?, ?);", -1, &stmt, NULL);
	SQLStmtHolder ins(stmt);
	if (ret != SQLITE_OK) {
		std::cerr << "db prepare failed: " << sqlite3_errstr(ret) <<
			" -> " << sqlite3_errmsg(sql_holder) << "\n";
		return EXIT_FAILURE;
	}

	int rnd = rand() % 1000;
	ret = sqlite3_bind_int(ins, 1, rnd);
	if (ret != SQLITE_OK) {
		std::cerr << "db bind failed: " << sqlite3_errstr(ret) <<
			" -> " << sqlite3_errmsg(sql_holder) << "\n";
		return EXIT_FAILURE;
	}

	ret = sqlite3_bind_text(ins, 2, "bubak", -1, SQLITE_STATIC);
	if (ret != SQLITE_OK) {
		std::cerr << "db bind failed: " << sqlite3_errstr(ret) <<
			" -> " << sqlite3_errmsg(sql_holder) << "\n";
		return EXIT_FAILURE;
	}

	ret = sqlite3_step(ins);
	if (ret != SQLITE_DONE) {
		std::cerr << "db step ins(" << rnd << ") failed: " <<
			sqlite3_errstr(ret) <<
			" -> " << sqlite3_errmsg(sql_holder) << "\n";
		return EXIT_FAILURE;
	}

	return 0;
}

