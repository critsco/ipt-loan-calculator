#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <jansson.h>
#include <string.h>
#include <fcgi_stdio.h>

//gcc -o s_remove.o s_remove.c -lsqlite3 -ljansson  -lfcgi
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

    int result;
    const char *deleteId;

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
        uid = json_string_value(json_object_get(value, "value"));

        result = strcmp(name, "selectedRow");

        if (result == 0) {
            deleteId = uid;
            printf("name: %s\n", name);
            printf("value: %s\n", deleteId);
        }       
    }

    rc = sqlite3_open("db/data.db", &db);
    
    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    const char *sql = "DELETE FROM students WHERE s_id = ?";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    sqlite3_bind_text(stmt, 1, deleteId, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    json_object_set_new(res_root, "status", json_string("success"));
    json_object_set_new(res_root, "message", json_string("Row deleted successfully"));

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // Convert the JSON response to a string    
    response_data = json_dumps(res_root, JSON_INDENT(4));
   
    printf("Content-Type: application/json\n\n");    
    printf("%s", response_data);

    // Free the allocated memory
    free(response_data);
    free(incoming_data);

    return 0;

   }

}

//curl -X POST -H "Content-Type: application/json" -d '[{"name": "selectedRow", "value": "21100001011"}]' http://172.28.98.30:8080/test/test/cgi-bin/s_remove.o

