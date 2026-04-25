#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void print_all(buffer *buffer) {
    line *line_to_print = buffer->head;
    while (line_to_print != NULL) {
        printf("%s", line_to_print->text);
        line_to_print = line_to_print->next;
    }
    printf("\n");
}

void free_line(line *line) {
    free(line->text);
    free(line);
}

void delete_lines(buffer *buffer, int start, int amount) {
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
    line *new_line = malloc(sizeof(line));
    new_line->next = NULL;
    new_line->prev = NULL;
    
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

    char line_buf[1024];
    fgets(line_buf, sizeof(line_buf), stdin);
    new_line->text = strdup(line_buf);
}

void argument_parser(char *command_buf, buffer *buffer) {
    // TODO:
    // argument_parser
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

    char command_buf[1024];
    print_all(buffer);
    insert(buffer);
    insert(buffer);
    print_all(buffer);
    while (1) {
        fgets(command_buf, sizeof(command_buf), stdin);
        argument_parser(command_buf, buffer);
    }
}
