#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

void parseJSON(cJSON *json, FILE *csvFile) {
    if (json == NULL) {
        return;
    }

    if (json->type == cJSON_Array) {
        cJSON *child = json->child;
        if (child) {
            // Записываем заголовки столбцов на первой итерации
            cJSON *object = child;
            cJSON *item = object->child;
            while (item) {
                if (item->type == cJSON_String) {
                    fprintf(csvFile, "%s", item->string);
                } else if (item->type == cJSON_Number) {
                    fprintf(csvFile, "%s", cJSON_Print(item));
                }

                item = item->next;
                if (item) {
                    fprintf(csvFile, ",");
                }
            }
            fprintf(csvFile, "\n");

            // Записываем значения по соответствующим ключам
            while (object) {
                item = object->child;
                while (item) {
                    if (item->type == cJSON_Object || item->type == cJSON_Array) {
                        fprintf(csvFile, "[Object/Array]");
                    } else if (item->type == cJSON_String) {
                        fprintf(csvFile, "%s", item->valuestring);
                    } else if (item->type == cJSON_Number) {
                        fprintf(csvFile, "%s", cJSON_Print(item));
                    } else if (item->type == cJSON_True) {
                        fprintf(csvFile, "1");
                    } else if (item->type == cJSON_False) {
                        fprintf(csvFile, "0");
                    }

                    item = item->next;
                    if (item) {
                        fprintf(csvFile, ",");
                    }
                }

                fprintf(csvFile, "\n");
                object = object->next;
            }
        }
    }
}

int main() {
    printf("Enter JSON-file: ");
    char filename[100];
    fgets(filename, sizeof(filename), stdin);

    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }

    FILE *jsonFile = fopen(filename, "r");
    if (jsonFile == NULL) {
        printf("Error open JSON.\n");
        return 1;
    }

    fseek(jsonFile, 0, SEEK_END);
    long fileSize = ftell(jsonFile);
    rewind(jsonFile);

    char *jsonString = (char *)malloc(fileSize + 1);
    if (jsonString == NULL) {
        printf("Memory error.\n");
        fclose(jsonFile);
        return 1;
    }

    fread(jsonString, 1, fileSize, jsonFile);
    jsonString[fileSize] = '\0';
    fclose(jsonFile);

    cJSON *json = cJSON_Parse(jsonString);
    if (json == NULL) {
        printf("Error parcing JSON.\n");
        free(jsonString);
        return 1;
    }

    free(jsonString);

    FILE *csvFile = fopen("output.csv", "w");
    if (csvFile == NULL) {
        printf("Error open CSV.\n");
        cJSON_Delete(json);
        return 1;
    }

    parseJSON(json, csvFile);

    cJSON_Delete(json);
    fclose(csvFile);

    printf("Transformation JSON to CSV complete.\n");

    return 0;
}
