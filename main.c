#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <ctype.h>

#define MAX_PASSWORD_LENGTH 128
#define MAX_PURPOSE_LENGTH 100
#define MASTER_FILE "data/master.txt"
#define PASSWORD_FILE "data/password_list.txt"

const char *lower = "abcdefghijklmnopqrstuvwxyz";
const char *upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *digits = "0123456789";
const char *symbols = "!@#$%^&*()_+-=[]{}|;:,.<>?";

void generate_password(char *password, int length)
{
    int i = 0;
    int index = 0;
    int lower_len = strlen(lower);
    int upper_len = strlen(upper);
    int digits_len = strlen(digits);
    int symbols_len = strlen(symbols);

    password[index++] = lower[rand() % lower_len];
    password[index++] = upper[rand() % upper_len];
    password[index++] = digits[rand() % digits_len];
    password[index++] = symbols[rand() % symbols_len];

    const char *all_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?";
    int all_len = strlen(all_chars);

    for (i = index; i < length; i++)
    {
        password[i] = all_chars[rand() % all_len];
    }

    for (i = 0; i < length; i++) // Here we are using Fisher-Yates shuffle to randomize the pattern of the final password.
    {
        int j = rand() % length; // The rand() % length is working as a randomizer to chose an index.
        char temp = password[i];
        password[i] = password[j];
        password[j] = temp;
    }

    password[length] = '\0';
}

void xor_encrypt_decrypt(char *text, const char *key)
{
    size_t text_len = strlen(text);
    size_t key_len = strlen(key);
    for (size_t i = 0; i < text_len; i++)
    {
        text[i] ^= key[i % key_len];
    }
}

void save_to_file(const char *purpose, const char *encrypted_password)
{
    FILE *file = fopen(PASSWORD_FILE, "a");
    if (file == NULL)
    {
        perror("Error opening password file");
        return;
    }

    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0';

    fprintf(file, "%s|%s|%s\n", purpose, encrypted_password, timestamp);
    fclose(file);
}

int check_master_password(char *master_key)
{
    FILE *file = fopen(MASTER_FILE, "r");
    char stored_pass[100];
    char input_pass[100];

    if (file == NULL)
    {
        printf("No master password set. Please set one now: ");
        scanf("%99s", stored_pass);

        file = fopen(MASTER_FILE, "w");
        if (file == NULL)
        {
            perror("Error creating master password file");
            return 0;
        }
        fprintf(file, "%s", stored_pass);
        fclose(file);
        strcpy(master_key, stored_pass);
        printf("Master password set successfully.\n");
        return 1;
    }

    fscanf(file, "%99s", stored_pass);
    fclose(file);

    printf("Enter Master Password: ");
    scanf("%99s", input_pass);

    if (strcmp(stored_pass, input_pass) == 0)
    {
        strcpy(master_key, input_pass);
        return 1;
    }
    else
    {
        printf("Incorrect master password. Access denied.\n");
        return 0;
    }
}

void view_passwords(const char *master_key)
{
    FILE *file = fopen(PASSWORD_FILE, "r");
    if (file == NULL)
    {
        printf("No passwords saved yet.\n");
        return;
    }

    char line[512];
    char purpose[MAX_PURPOSE_LENGTH];
    char encrypted_pass[MAX_PASSWORD_LENGTH];
    char timestamp[100];
    char decrypted_pass[MAX_PASSWORD_LENGTH];

    printf("\nSaved Passwords:\n---------------------------\n");
    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%99[^|]|%127[^|]|%99[^\n]", purpose, encrypted_pass, timestamp) == 3)
        {
            strcpy(decrypted_pass, encrypted_pass);
            xor_encrypt_decrypt(decrypted_pass, master_key);
            printf("[%s] -> %s | Created: %s\n", purpose, decrypted_pass, timestamp);
        }
    }
    fclose(file);
}

int main()
{
    int choice, length;
    char password[MAX_PASSWORD_LENGTH];
    char purpose[MAX_PURPOSE_LENGTH];
    char encrypted_password[MAX_PASSWORD_LENGTH];
    char master_key[100];

    struct stat st = {0};
    if (stat("data", &st) == -1)
    {
        mkdir("data");
    }

    srand((unsigned int)time(NULL));

    if (!check_master_password(master_key))
    {
        return 1;
    }

    printf("\nPassword Manager CLI\n---------------------\n");
    printf("1. Create new password\n");
    printf("2. View saved passwords\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice)
    {
    case 1:
        printf("Enter the purpose of the password (e.g., Google, Facebook, etc.): ");
        fgets(purpose, sizeof(purpose), stdin);
        purpose[strcspn(purpose, "\n")] = 0;

        printf("Enter desired password length (min 8, max %d): ", MAX_PASSWORD_LENGTH - 1);
        scanf("%d", &length);

        if (length < 8 || length >= MAX_PASSWORD_LENGTH)
        {
            printf("Invalid password length.\n");
            return 1;
        }

        generate_password(password, length);
        strcpy(encrypted_password, password);
        xor_encrypt_decrypt(encrypted_password, master_key);
        save_to_file(purpose, encrypted_password);
        printf("Password saved successfully.\n");
        break;

    case 2:
        view_passwords(master_key);
        break;

    default:
        printf("Invalid choice.\n");
    }

    return 0;
}
