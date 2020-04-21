#include "grid.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
extern int showSize;

// Todo: pass in show id
effect* getShow(void) {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;
    struct effect* show;
    int i = 0;
    try {
        // Create a connection
        driver = get_driver_instance();
        con = driver->connect("tcp://54.175.169.144:3306", "ubuntu", "natron3");
        con->setSchema("thegrid");

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT id,frame,name,color_start,color_end,beat,duration FROM effects WHERE show_id=1 ORDER BY beat DESC;");
        show = new effect[res->rowsCount()];
        // Add results to effects []
        while (res->next()) {
            show[i].id = (int)res->getInt("id");
            show[i].name = std::string((char*)res->getString("name").c_str());
            show[i].frCol = (int)res->getInt("color_start");
            show[i].toCol = (int)res->getInt("color_end");
            show[i].beat = (int)res->getInt("beat");
            show[i].frame = (int)res->getInt("frame");
            show[i].duration = (int)res->getInt("duration");
            // cout << show[i].name << endl;
            i++;
        }
        showSize = i;
        delete res;
        delete stmt;
        delete con;
    } catch (sql::SQLException &e) {
        show = new effect[1];
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
    printf("Returned %d effects.\n",sizeof(show));
    return show;
}

/*
int main(void)
{
cout << endl;
cout << "Running 'SELECT 'Hello World!' AS _message'..." << endl;

try {
  sql::Driver *driver;
  sql::Connection *con;
  sql::Statement *stmt;
  sql::ResultSet *res;

  // Create a connection
  driver = get_driver_instance();
  con = driver->connect("tcp://52.201.250.137:3306", "ubuntu", "natron3");
  // Connect to the MySQL test database
  con->setSchema("thegrid");

  stmt = con->createStatement();
  res = stmt->executeQuery("SELECT 'Hello World!' AS _message");
  while (res->next()) {
    cout << "\t... MySQL replies: ";
    // Access column data by alias or column name
    cout << res->getString("_message") << endl;
    cout << "\t... MySQL says it again: ";
    // Access column data by numeric offset, 1 is the first column
    cout << res->getString(1) << endl;
  }
  delete res;
  delete stmt;
  delete con;

} catch (sql::SQLException &e) {
  cout << "# ERR: SQLException in " << __FILE__;
  cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
  cout << "# ERR: " << e.what();
  cout << " (MySQL error code: " << e.getErrorCode();
  cout << ", SQLState: " << e.getSQLState() << " )" << endl;
}

cout << endl;

return EXIT_SUCCESS;
}
*/
