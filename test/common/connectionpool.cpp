#include <iostream>
#include <string>
#include <mysqlx/xdevapi.h>
#include "common/connection_pool.h"

using namespace mysqlx;
using namespace xKBEngine;

KBE_SINGLETON_INIT(ConnectionPool<Session>);

/*
CREATE DATABASE test;

CREATE TABLE IF NOT EXISTS `student`(
`id` INT UNSIGNED AUTO_INCREMENT,
`name` VARCHAR(40) NOT NULL,
`age` INT UNSIGNED,
PRIMARY KEY ( `id` )
)DEFAULT CHARSET=utf8;

INSERT INTO test (name, age) VALUES ("fwd", 27);
INSERT INTO test (name, age) VALUES ("xb", 25);
*/

int main()
{
	ConnectionPool<Session> pool(5, 10, "mysqlx://root:12345678@localhost/test");
	auto conn = ConnectionPool<Session>::getSingleton().get();
	Schema db = conn->get().getSchema("test");
	Table table = db.getTable("student");

	RowResult ret = table.select("id", "name", "age")
		.where("name like :name AND age < :age")
		.bind("name", "f%").bind("age", 60).execute();

	Row row = ret.fetchOne();
	std::cout << "      id: " << row[0] << std::endl;
	std::cout << "    name: " << row[1] << std::endl;
	std::cout << "     age: " << row[2] << std::endl;

    return 0;
}