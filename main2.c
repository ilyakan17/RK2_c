#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

cJSON *createJSONObjectFromCSV(char *csvRow, char **columnHeaders, int numColumns) {
    cJSON *object = cJSON_CreateObject();

    char *token = strtok(csvRow, ",");
    int columnIndex = 0;

    while (token != NULL && columnIndex < numColumns) {
        cJSON *value;

        // Попытка преобразовать значение в число
        int intValue = atoi(token);
        if (intValue != 0 || strcmp(token, "0") == 0) {
            value = cJSON_CreateNumber(intValue);
        } else {
            value = cJSON_CreateString(token);
        }

        cJSON_AddItemToObject(object, columnHeaders[columnIndex], value);

        token = strtok(NULL, ",");
        columnIndex++;
    }

    return object;
}

cJSON *parseCSV(const char *csvFilename) {
    FILE *csvFile = fopen(csvFilename, "r");
    if (csvFile == NULL) {
        printf("Error open CSV-file.\n");
        return NULL;
    }

    // Чтение заголовков столбцов
    char line[1024];
    if (fgets(line, sizeof(line), csvFile) == NULL) {
        printf("Error of reading header.\n");
        fclose(csvFile);
        return NULL;
    }

    // Разделение заголовков столбцов по запятой
    char **columnHeaders = NULL;
    int numColumns = 0;
    char *token = strtok(line, ",");
    while (token != NULL) {
        numColumns++;
        columnHeaders = realloc(columnHeaders, numColumns * sizeof(char *));
        columnHeaders[numColumns - 1] = strdup(token);

        token = strtok(NULL, ",");
    }

    // Создание корневого JSON-массива
    cJSON *jsonArray = cJSON_CreateArray();

    // Чтение строк данных и создание JSON-объектов
    while (fgets(line, sizeof(line), csvFile) != NULL) {
        cJSON *jsonObject = createJSONObjectFromCSV(line, columnHeaders, numColumns);
        cJSON_AddItemToArray(jsonArray, jsonObject);
    }

    fclose(csvFile);

    // Освобождение памяти заголовков столбцов
    for (int i = 0; i < numColumns; i++) {
        free(columnHeaders[i]);
    }
    free(columnHeaders);

    return jsonArray;
}

int main() {
    printf("Enter CSV-file: ");
    char filename[100];
    fgets(filename, sizeof(filename), stdin);

    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }

    cJSON *json = parseCSV(filename);
    if (json == NULL) {
        printf("Transformation error CSV in JSON.\n");
        return 1;
    }

    // Преобразование JSON в строку
    char *jsonString = cJSON_Print(json);

    // Создание выходного файла JSON
    char outputFilename[100];
    snprintf(outputFilename, sizeof(outputFilename), "%s.json", filename);
    FILE *outputFile = fopen(outputFilename, "w");
    if (outputFile == NULL) {
        printf("Error create JSON.\n");
        cJSON_Delete(json);
        free(jsonString);
        return 1;
    }

    // Запись JSON-строки в файл
    fprintf(outputFile, "%s\n", jsonString);
    fclose(outputFile);

    printf("JSON-file is created: %s\n", outputFilename);

    // Освобождение памяти
    cJSON_Delete(json);
    free(jsonString);

    return 0;
}
