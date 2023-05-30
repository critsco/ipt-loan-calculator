#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <jansson.h>
#include <string.h>

//gcc -o s_update.o s_update.c -lsqlite3 -ljansson

typedef struct {
    char* s_id;
    char* s_name;
    char* s_sex;
    char* s_contact;
    char* s_address;
    char* s_dept;
    char* s_course;
    char* s_year;
    char* s_section;
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

void loadJSON(Update* students) {
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
            students->s_id = strdup(item_value);
        } else if (strcmp(name, "e_name") == 0) {
            students->s_name = strdup(item_value);
        } else if (strcmp(name, "e_sex") == 0) {
            students->s_sex = strdup(item_value);
        } else if (strcmp(name, "e_contact") == 0) {
            students->s_contact = strdup(item_value);
        } else if (strcmp(name, "e_address") == 0) {
            students->s_address = strdup(item_value);
        } else if (strcmp(name, "e_dept") == 0) {
            students->s_dept = strdup(item_value);
        } else if (strcmp(name, "e_course") == 0) {
            students->s_course = strdup(item_value);
        } else if (strcmp(name, "e_year") == 0) {
            students->s_year = strdup(item_value);
        } else if (strcmp(name, "e_section") == 0) {
            students->s_section = strdup(item_value);
        }
    }

    json_decref(root);
}

int main(void) {
    Update students;

    // Load JSON payload from client-side and deserialize
    loadJSON(&students);

    sqlite3* db;
    sqlite3_stmt* stmt;

    // Open the database
    int dbres = sqlite3_open("db/data.db", &db);
    if (dbres != SQLITE_OK) {
        create_json_error_response("Unable to open the database.");
        return 1;
    }

    // Prepare the SQL statement
    const char* sql_statement = "UPDATE students SET s_name = ?, s_sex = ?, s_contact = ?, s_address = ?, s_dept = ?, s_course = ?, s_year = ?, s_section = ? WHERE s_id = ?";
    dbres = sqlite3_prepare_v2(db, sql_statement, -1, &stmt, NULL);
    if (dbres != SQLITE_OK) {
        create_json_error_response("Unable to prepare the SQL statement.");
        sqlite3_close(db);
        return 1;
    }

    // Bind the values to the SQL statement
    dbres = sqlite3_bind_text(stmt, 9, students.s_id, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 1, students.s_name, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 2, students.s_sex, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 3, students.s_contact, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 4, students.s_address, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 5, students.s_dept, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 6, students.s_course, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 7, students.s_year, -1, SQLITE_TRANSIENT);
    dbres |= sqlite3_bind_text(stmt, 8, students.s_section, -1, SQLITE_TRANSIENT);

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
    free(students.s_id);
    free(students.s_name);
    free(students.s_sex);
    free(students.s_contact);
    free(students.s_address);
    free(students.s_dept);
    free(students.s_course);
    free(students.s_year);
    free(students.s_section);

    return 0;
}

// curl -X POST -H "Content-Type: application/json" -d '[{"name": "e_id", "value": "2110000"},{"name": "e_name", "value": "student name"},{"name": "e_sex", "value": "Male"},{"name": "e_contact", "value": "12345678910"},{"name": "e_address", "value": "student address"},{"name": "e_dept", "value": "student dept"},{"name": "e_course", "value": "student course"},{"name": "e_year", "value": "2"},{"name": "e_section", "value": "student section"}]' http://172.28.98.30:8080/test/test/cgi-bin/s_update.o