#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <jansson.h>
#include <string.h>
#include <fcgi_stdio.h>

//gcc -o s_search.o s_search.c -lsqlite3 -ljansson  -lfcgi
//sudo apt-get install libapache2-mod-fcgid
//sudo apt-get install libfcgi-dev

int main() {
 while (FCGI_Accept() >= 0) {
    /* for SQLite3 */
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;
  
    /* use for checking and root data [incoming request] */
    long len = 0; // use to measure the length of incoming data
    json_t *root;
    json_error_t error;

    json_t *object; // for json_object();
    json_t *res_root = json_object(); /* for combining */
    json_t *array = json_array(); /* for combining */

    
    char *response_data; /* return response */    
    char *incoming_data; /* original request */

    /* use for json_array_foreach */
    size_t index;
    json_t *value;
    const char *name;
    const char *uid;

    // int id;
    int result;


    // Get the length of the input data
    len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
   
    // Allocate memory to store the input data
    incoming_data = malloc(len+1);

    // Read the input data from the standard input stream
    fgets(incoming_data, len + 1, stdin);

    // Parse the input data as JSON
    root = json_loads(incoming_data, 0, &error);

    if(!root) {
        printf("error: on line %d: %s\n", error.line, error.text);
        return 0;
    }

   
    // Extract the required information from the JSON data
    json_array_foreach(root, index, value) {
        name = json_string_value(json_object_get(value, "name"));
        // id = atoi(json_string_value (json_object_get(value, "value"))); // atoi convert string to int
        uid = json_string_value (json_object_get(value, "value")); // atoi convert string to int
        
       result = strcmp(name, "name");

        // if (result == 0) {       
            // uid = id;
            printf("name: %s\n", name);
            printf("value: %s\n", uid);
        // }       
    }

    char *searchValue = malloc(strlen(uid) + 2);
    strcpy(searchValue, "%");
    strcat(searchValue, uid);
    strcat(searchValue, "%");

    rc = sqlite3_open("db/data.db", &db);
    
    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    const char *sql = "SELECT * FROM students WHERE s_name LIKE ?";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // sqlite3_bind_int(stmt, 1, uid);
    sqlite3_bind_text(stmt, 1, searchValue, -1, SQLITE_STATIC);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    
            object = json_object();

            json_object_set_new(object, "s_id", json_string((const char*)sqlite3_column_text(stmt, 0)));
            json_object_set_new(object, "s_name", json_string((const char*)sqlite3_column_text(stmt, 1)));
            json_object_set_new(object, "s_sex", json_string((const char*)sqlite3_column_text(stmt, 2)));
            json_object_set_new(object, "s_contact", json_string((const char*)sqlite3_column_text(stmt, 3)));
            json_object_set_new(object, "s_address", json_string((const char*)sqlite3_column_text(stmt, 4)));
            json_object_set_new(object, "s_dept", json_string((const char*)sqlite3_column_text(stmt, 5)));
            json_object_set_new(object, "s_course", json_string((const char*)sqlite3_column_text(stmt, 6)));
            json_object_set_new(object, "s_year", json_string((const char*)sqlite3_column_text(stmt, 7)));
            json_object_set_new(object, "s_section", json_string((const char*)sqlite3_column_text(stmt, 8)));
           
            // add the JSON object to the JSON array
            json_array_append_new(array, object);
             
    }

    json_object_set_new(res_root, "status", json_string("success"));
    json_object_set_new(res_root, "message", json_string(incoming_data));
    json_object_set_new(res_root, "data", array);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // Convert the JSON response to a string    
    response_data = json_dumps(res_root,  JSON_INDENT(4)); //JSON_INDENT(4));
   
    printf("Content-Type: application/json\n\n");    
    printf("%s", response_data);

    // Free the allocated memory
    free(response_data);
    free(incoming_data);
    free(searchValue);

    return 0;

   }

}

//curl -X POST -H "Content-Type: application/json" -d '[{"name": "name", "value": "James"}]' http://172.28.98.30:8080/test/test/cgi-bin/s_search.o
