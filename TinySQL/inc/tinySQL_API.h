/*
 * tinySQL_API.h
 *
 *  Created on: 9 Feb 2014
 *      Author: murphy
 */

#ifndef TINYSQL_API_H_
#define TINYSQL_API_H_


tinySQL_CREATE(database_name, new_table, num_columns, column_names, column_types);
tinySQL_INSERT(database_name, table_name, num_value, values, columns);
tinySQL_DELETE(table_name, expr);
tinySQL_DROP();
tinySQL_SELECT(database_name, table_name, column, void *match, value);
tinySQL_UPDATE();
tinySQL_ALTER();

#endif /* TINYSQL_API_H_ */
