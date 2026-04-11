#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* errorString(int errorCode);

const char filePath[] = "C:\\Users\\xaver\\Documents\\2025_26_codes\\par_dev_programming\\c_sdk_220203\\MinGW\\include\\CL\\cl.h";

int main(int argc, char const *argv[])
{
    int errorCode = -33;
    const char *errorName = errorString(errorCode);
    printf("Error code: %d, Error name: %s\n", errorCode, errorName ? errorName : "Unknown error");
    return 0;
}

const char* errorString(int errorCode)
{
    FILE *file = fopen(filePath, "r");
    if (file == NULL)
    {
        printf("Error opening file: %s\n", filePath);
        return NULL;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "#define") && strstr(line, "CL_") && strstr(line, " -") && strstr(line, " "))
        {
            char *token = strtok(line, " ");
            token = strtok(NULL, " ");
            const char *errorName = token;
            token = strtok(NULL, " ");
            int code = atoi(token);

            if (code == errorCode)
            {
                fclose(file);
                return errorName;
            }
        }
    }

    fclose(file);
    return NULL;
}