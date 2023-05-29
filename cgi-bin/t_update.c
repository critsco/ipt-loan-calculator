#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <jansson.h>
#include <string.h>

//gcc -o t_update.o t_update.c -lsqlite3 -ljansson

typedef struct {
    char* t_id;
    char* t_name;
    char* t_sex;
    char* t_contact;
    char* t_address;
    char* t_dept;
} Update;

void create_json_error_response(const char* err_msg) {
    json_t* response = json_object();
    json_object_set_new(response, "status", json_string("error"));
    json_object_set_new(response, "message", json_string(err_msg));
    char* response_data = json_dumps(response, JSON_INDENT(4));
    printf("Content-Type: application/json\n\n");
    printf("%s", response_data);
    free(response_data);
    json_decref(response);
}

void loadJSON(Update* teachers) {
    long len = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
    char* incoming_data = malloc(len + 1);

    // Read the input data from the standard input stream
    fgets(incoming_data, len + 1, stdin);

    json_error_t error;
    json_t* root = json_loads(incoming_data, 0, &error);
    free(incoming_data);

    if (!root) {
        create_json_error_response(error.text);
        return;
    }

    size_t index;
    json_t* value;
    json_array_foreach(root, index, value) {
        const char* name = json_string_value(json_object_get(value, "name"));
        const char* item_value = json_string_value(json_object_get(value, "value"));

        if (strcmp(name, "e_id") == 0) {
            teachers->t_id = strdup(item_value);
        } else if (strcmp(name, "e_name") == 0) {
            teachers->t_name = strdup(item_value);
        } else if (strcmp(name, "e_sex") == 0) {
            teachers->t_sex = strdup(item_value);
        } else if (strcmp(name, "e_contact") == 0) {
            teachers->t_contact = strdup(item_value);
        } else if (strcmp(name, "e_address") == 0) {
            teachers->t_address = strdup(item_value);
        } else if (strcmp(name, "e_dept") == 0) {
            teachers->t_dept = strdup(item_value);
        }
    }

    json_decref(root);
}

int main(void) {
    Update teachers;

    // Load JSON payload from client-side and deserialize
    loadJSON(&teachers);

    sqlite3* db;
    sqlite3_stmt* stmt;

    // Open the database
    int dbres = sqlite3_open("db/data.db", &db);
    if (dbres != SQLITE_OK) {
        create_json_error_response("Unable to open the database.");
        return 1;
    }

    // Prepare the SQL statement
    const char* sql_statement = "UPDATE teachers SET t_name = ?, t_sex = ?, t_contact = ?, t_address = ?, t_dept = ? WHERE t_id = ?";
    dbres = sqlite3_prepare_v2(db, sql_statement, -1, &stmt, NULL);
    if (dbres != SQLITE_OK) {
        create_json_error_response("Unable to prepare the SQL statement.");
        sqlite3_close(db);
        return 1;
    }

    // Bind the values to the SQL statement
    dbres = sqlite3_bind_text(stmt, 6, teachers.t_id, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 1, teachers.t_name, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 2, teachers.t_sex, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 3, teachers.t_contact, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 4, teachers.t_address, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 5, teachers.t_dept, -1, SQLITE_TRANSIENT);

    if (dbres != SQLITE_OK) {
        create_json_error_response("Unable to bind the values to the SQL statement.");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }

    // Execute the SQL statement
    dbres = sqlite3_step(stmt);
    if (dbres != SQLITE_DONE) {
        create_json_error_response("Unable to execute the SQL statement.");
        create_json_error_response(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }

    // Finalize the SQL statement and close the database
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // Send the success response
    json_t* response = json_object();
    json_object_set_new(response, "status", json_string("success"));
    json_object_set_new(response, "message", json_string("Successfully updated the data."));
    char* response_data = json_dumps(response, JSON_INDENT(4));
    printf("Content-Type: application/json\n\n");
    printf("%s", response_data);
    free(response_data);
    json_decref(response);

    // Free the allocated memory
    free(teachers.t_id);
    free(teachers.t_name);
    free(teachers.t_sex);
    free(teachers.t_contact);
    free(teachers.t_address);
    free(teachers.t_dept);

    return 0;
}

// curl -X POST -H "Content-Type: application/json" -d '[{"name": "e_id", "value": "21100001011"},{"name": "e_name", "value": "teacher name"},{"name": "e_sex", "value": "Male"},{"name": "e_contact", "value": "12345678910"},{"name": "e_address", "value": "teacher address"},{"name": "e_dept", "value": "teacher dept"}]' http://172.28.98.30:8080/test/test/cgi-bin/t_update.o