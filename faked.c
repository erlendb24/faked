#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NO_ARGS 0
#define SINGLE_ARG 1
#define RANGE 2

typedef struct line {
    char *text;
    struct line *prev;
    struct line *next;
} line;

typedef struct buffer {
    line *head;
    line *tail;
    line *current;
    int is_changed;
    char *filename;
} buffer;


/* TODO:
 * void print_all_lines(buffer)
 * void insert(buffer, number)
 * void remove_lines(buffer, number, amount)
 * void save_to_file(buffer)
 * void change_line
 * */

void save_to_file(buffer *buffer) {
    FILE *fp = fopen(buffer->filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "File could not be opened, try again!");
        return;
    }

    line *tmp_line = buffer->head;
    while (tmp_line != NULL) {
        fputs(tmp_line->text, fp);
        tmp_line = tmp_line->next;
    }
    fclose(fp);
}

int find_current(buffer *buffer) {
    line *tmp_line = buffer->head;
    int counter = 1;
    while (tmp_line != buffer->current) {
        tmp_line = tmp_line->next;
        counter++;
    }
    return counter;
}

void set_current(buffer *buffer, int line_nr) {
    line *tmp_line = buffer->head;
    int counter = 0;
    while (counter < line_nr) {
        tmp_line = tmp_line->next;
        counter++;
    }

    buffer->current = tmp_line;
}

int find_tail(buffer *buffer) {
    line *tmp_line = buffer->head;
    int counter = 1;
    while (tmp_line != buffer->tail) {
        tmp_line = tmp_line->next;
        counter++;
    }
    return counter;
}

void print_all(buffer *buffer) {
    line *line_to_print = buffer->head;
    while (line_to_print != NULL) {
        printf("%s", line_to_print->text);
        line_to_print = line_to_print->next;
    }
    printf("\n");
}

void print_lines(buffer *buffer, int start, int amount) {
    int counter = 1;
    line *tmp_line = buffer->head;
    while (counter < start) {
        tmp_line = tmp_line->next;
        counter++;
    }
    for (int i = 0; i < amount; i++) {
        while (tmp_line != NULL) {
            printf("%s", tmp_line->text);
            tmp_line = tmp_line->next;
        }
    }
    printf("\n");
}

void free_line(line *line) {
    free(line->text);
    free(line);
}

void delete_lines(buffer *buffer, int start, int amount) {
    buffer->is_changed = 1;
    // start at one because first line is 1
    int counter = 1;
    line *tmp_line = buffer->head;
    while (counter < start) {
        tmp_line = tmp_line->next;
        counter++;
    }

    for (int i = 0; i < amount; i++) {
        line *line_to_delete = tmp_line;
        tmp_line = tmp_line->next;
        if (line_to_delete->prev != NULL) {
            line_to_delete->prev->next = tmp_line;
        } else {
            buffer->head = tmp_line;
        }
        if (tmp_line != NULL) {
            tmp_line->prev = line_to_delete->prev;
        }
        free_line(line_to_delete);
    }
    buffer->current = tmp_line;
}

// Insert text into the line before current
void insert(buffer *buffer) {
    buffer->is_changed = 1;
    line *new_line = malloc(sizeof(line));
    new_line->next = NULL;
    new_line->prev = NULL;
    char line_buf[1024];
    fgets(line_buf, sizeof(line_buf), stdin);
    new_line->text = strdup(line_buf);

    if (buffer->current == NULL) {
        buffer->current = new_line;
        buffer->tail->next = new_line;
        new_line->prev = buffer->tail;
        buffer->tail = new_line;
        return;
    }
    // If there is a previous, make new_line's previous that line
    if (buffer->current->prev != NULL) {
        buffer->current->prev->next = new_line;
        new_line->prev = buffer->current->prev;
    } else {
        // If there is no previous (i.e. current is the only line in the file),
        // make new line the head of the buffer
        buffer->head = new_line;
    }

    // Set current's previous to new_line and new_line's next to current
    buffer->current->prev = new_line;
    new_line->next = buffer->current;
}

void resolve_change_and_delete(buffer *buffer, char type, int args[2], int address_space) {
    if (address_space == SINGLE_ARG) {
        delete_lines(buffer, args[0], SINGLE_ARG);
    } else if (address_space == RANGE) {
        delete_lines(buffer, args[0], (args[1] - args[0]) + 1);
    } else {
        delete_lines(buffer, find_current(buffer), 1);
    }

    if (type == 'c') {
        insert(buffer);
    }
}

void resolve_current_for_append(buffer *buffer, int args[2], int address_space) {
    if (address_space == SINGLE_ARG) {
        if (args[0] != find_tail(buffer) || args[0] != find_current(buffer)) {
            set_current(buffer, args[0]);
        } else {
            if (args[0] == find_tail(buffer)) {
                buffer->current = buffer->tail->next;
            }
            if (args[0] == find_current(buffer)) {
                // sets current to line after current, so you append after
                set_current(buffer, args[0] + 1);
            }
        }
    }
}

void resolve_print(buffer *buffer, int args[2], int address_space, int arg_amount) {
    if (address_space == RANGE) {
        // if its 1, it has to be ',p' which prints all lines
        if (arg_amount > 1) {
            print_lines(buffer, args[0], (args[1] - args[0]) + 1);
        } else {
            print_all(buffer);
        }
    } else if (address_space == SINGLE_ARG) {
        print_lines(buffer, args[0], SINGLE_ARG);
    } else {
        printf("%s", buffer->current->text);
    }
}

void handle_unsaved(buffer *buffer) {
    char line_buf[1024];

    if (buffer->is_changed == 1) {
        printf("%s", "Unsaved changes! Are you sure? [Y/N]\n");
        fgets(line_buf, sizeof(line_buf), stdin);
        if (line_buf[0] == 'Y') {
            exit(EXIT_SUCCESS);
        } else {
            return;
        }
    } else {
        exit(EXIT_SUCCESS);
    }
}

void argument_parser(char *command_buf, buffer *buffer) {
    int args[2] = { 0 };
    int arg_counter = 0;
    char command = 0;
    int address_space = SINGLE_ARG;
    for (int i = 0; command_buf[i] != '\0'; i++) {
        if (!isalpha(command_buf[i])) {
            if (isdigit(command_buf[i])) {
                int num = command_buf[i] - '0';
                args[arg_counter] = args[arg_counter] * 10 + num;
            }
            if (command_buf[i] == '$') {
                args[arg_counter] = find_tail(buffer);
            }
            if (command_buf[i] == '.') {
                args[arg_counter] = find_current(buffer);
            }

            if (command_buf[i] == ',') {
                address_space = RANGE;
                arg_counter++;
            }
        } else {
            command = command_buf[i];
            if (i == 0) {
                address_space = NO_ARGS;
            }
        }
    }

    switch (command) {
        case 'd':
            resolve_change_and_delete(buffer, 'd', args, address_space);
            break;
        case 'c':
            resolve_change_and_delete(buffer, 'c', args, address_space);
            break;
        case 'i':
            insert(buffer);
            break;
        case 'a':
            resolve_current_for_append(buffer, args, address_space);
            insert(buffer);
            break;
        case 'P':
            print_all(buffer);
            break;
        case 'p':
            resolve_print(buffer, args, address_space, arg_counter);
            break;
        case 'w':
            save_to_file(buffer);
            break;
        case 'q':
            handle_unsaved(buffer);
        default:
            printf("Command: %s is not recognized", command_buf);
            break;
    }
}

int main(int argc, char **argv) {
    char *path = argv[1];
    FILE *fp = fopen(path, "r");
    char line_buf[1024];
    buffer *buffer = malloc(sizeof(struct buffer));
    buffer->head = NULL;
    buffer->tail = NULL;
    buffer->current = NULL;
    buffer->is_changed = 0;
    buffer->filename = strdup(path);
    // remember to strdup the buffer
    while (fgets(line_buf, sizeof(line_buf), fp)) {
        line *line = malloc(sizeof(struct line));
        char *duped_buf = strdup(line_buf);
        line->prev = NULL;
        line->next = NULL;
        line->text = duped_buf;
        if (buffer->current != NULL) {
            line->prev = buffer->current;
            buffer->current->next = line;
        }
        if (buffer->head == NULL) {
            buffer->head = line;
        }
        buffer->current = line;
    }
    buffer->tail = buffer->current;

    fclose(fp);
    char command_buf[1024];
    while (1) {
        fgets(command_buf, sizeof(command_buf), stdin);
        argument_parser(command_buf, buffer);
    }
}
