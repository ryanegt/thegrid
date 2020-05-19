#include "mysql.h"
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
int effectNum = 0;
int maxBeat = 0;

/*
* Get effects for a show id
*/
vector<vector<effect>> getEffects(show master) {
    sql::Driver * driver;
    sql::Connection * con;
    sql::Statement * stmt;
    sql::ResultSet * res;
    vector<vector<effect>> showEffects;
    int id = master.id;
    // printf("Initd double vector. Show id: %d\n",master.id);
    // printf("Num of beats: %d\n",master.beatTimes.size());
    int i = 0;
    try {
        // Create a connection
        driver = get_driver_instance();
        con = driver->connect("tcp://54.175.169.144:3306", "ubuntu", "natron3");
        con->setSchema("thegrid");

        // Make query for effect array
        stmt = con->createStatement();
        std::string query = "SELECT * FROM effects WHERE show_id="
            +std::to_string(id)+" ORDER BY beat ASC";

        res = stmt->executeQuery(query.c_str());
        showEffects.resize(master.beatTimes.size()*4);

        // Add results to effects []
        while (res->next()) {
            // Create second level of beat array
            effect e;
            int b = (int)res->getInt("beat");
            e.id = (int)res->getInt("id");
            e.name = std::string((char*)res->getString("name").c_str());
            e.frame = (int)res->getInt("frame");
            e.color = (int)res->getInt("color");
            e.beat = (int)res->getInt("measure");
            e.beat = (int)res->getInt("beat");
            e.measure = (int)res->getInt("measure");
            e.elapsed = (int)res->getInt("elapsed");
            e.duration = (int)res->getInt("duration");
            e.frequency = (int)res->getInt("frequency");
            // e.reverse = (int)res->getInt("reverse");
            maxBeat = b;
            // Add to multiple beats based on the duration
            for(int d = 0; d<e.duration; ++d) {
                printf("Adding id %d to show[%d]\n",(int)res->getInt("id"),(b+d));
                showEffects[b+d].push_back(e);
            }
            i++;
        }

        effectNum = i;
        delete res;
        delete stmt;
        delete con;
    } catch (sql::SQLException &e) {
        // showEffects = vector<vector<effect>(1)>(1);
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
    // printf("Returned %d effects.\n",sizeof(show));
    return showEffects;
}

show getShow(int id) {
    sql::Driver * driver;
    sql::Connection * con;
    sql::Statement * stmt;
    sql::ResultSet * res;
    Json::Value beatData;
    Json::Reader reader;
    show master;
    try {
        // Create a connection
        driver = get_driver_instance();
        con = driver->connect("tcp://54.175.169.144:3306", "ubuntu", "natron3");
        con->setSchema("thegrid");

        // Query to get show params
        stmt = con->createStatement();
        std::string query = "SELECT * FROM shows WHERE id="+std::to_string(id)+" LIMIT 1;";
        res = stmt->executeQuery(query.c_str());
        res->next();
        master.id = id;
        master.name = std::string((char*)res->getString("name").c_str());
        master.audioFile = std::string((char*)res->getString("audio_file").c_str());
        master.bpm = (float)res->getDouble("bpm");
        bool parsed = reader.parse( res->getString("timing").c_str(), beatData);     //parse process
        if (parsed){
            std::cout  << "Failed to parse" << reader.getFormattedErrorMessages();
        }
        // printf("JSON -------------:\n%s\n",(char*));
        for(unsigned int i=0; i<beatData["beat_times"].size(); ++i) {
            // printf("beattime[%d]: %f\n",i,beatData["beat_times"][i].asFloat());
            master.beatTimes.push_back(beatData["beat_times"][i].asFloat());
        }
        delete res;
        delete stmt;
        delete con;
    } catch (sql::SQLException &e) {
        // showEffects = vector<vector<effect>(1)>(1);
        cout << "# ERR : SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
    // printf("Returned %d effects.\n",sizeof(show));
    return master;
}
