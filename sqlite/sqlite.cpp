#include <cstdlib>
#include <iostream>
#include <memory>

#include <sqlite3.h>

using SQLUnique = std::unique_ptr<sqlite3, void (*)(sqlite3 *)>;
using SQLStmtUnique = std::unique_ptr<sqlite3_stmt, void (*)(sqlite3_stmt *)>;

struct SQLHolder : public SQLUnique {
	SQLHolder(sqlite3 *sql) : SQLUnique(sql, [](sqlite3 *sql) {
	      sqlite3_close(sql);
	}) {}

	operator sqlite3 *() { return get(); }
};

struct SQLStmtHolder : public SQLStmtUnique {
	SQLStmtHolder(sqlite3_stmt *stmt) : SQLStmtUnique(stmt, [](sqlite3_stmt *stmt) {
	      sqlite3_finalize(stmt);
	}) {}

	operator sqlite3_stmt *() { return get(); }
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

	for (int a = 0; a < 3; a++) {
		int rnd = rand() % 1000;
		ret = sqlite3_bind_int(ins, 1, rnd);
		if (ret != SQLITE_OK) {
			std::cerr << "db bind failed: " << sqlite3_errstr(ret) <<
				" -> " << sqlite3_errmsg(sql_holder) << "\n";
			return EXIT_FAILURE;
		}

		std::string bubak("bubak");
		bubak.append(std::to_string(a));
		ret = sqlite3_bind_text(ins, 2, bubak.c_str(), -1, SQLITE_STATIC);
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
		ret = sqlite3_reset(ins);
		if (ret != SQLITE_OK) {
			std::cerr << "db clear ins failed: " <<
				sqlite3_errstr(ret) <<
				" -> " << sqlite3_errmsg(sql_holder) << "\n";
			return EXIT_FAILURE;
		}
	}

	return 0;
}

