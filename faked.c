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

// Insert text into the line before current
void insert(buffer *buffer) {
    line *new_line = malloc(sizeof(line));

    
    // If there is a previous, make new_line's previous that line
    if (buffer->current->prev != NULL) {
        new_line->prev = buffer->current->prev;
    } else {
        // If there is no previous (i.e. current is the only line in the file),
        // make new line the head of the buffer
        buffer->head = new_line;
    }

    // Set current's previous to new_line and new_line's next to current
    buffer->current->prev = new_line;
    new_line->next = buffer->current;

    //TODO:
    //Input handling here?
}

void argument_parser(char *command_buf, buffer *buffer) {
    // TODO:
    // argument_parser
}

int main(int argc, char **argv) {
    char *path = argv[1];
    FILE *fp = fopen(path, "r");
    char line_buf[1024];
    buffer *buffer = malloc(sizeof(buffer));
    buffer->head = NULL;
    buffer->current = NULL;
    buffer->is_changed = 0;
    buffer->filename = path;
    // remember to strdup the buffer
    while (fgets(line_buf, sizeof(line_buf), fp)) {
        line *line = malloc(sizeof(line));
        char *duped_buf = strdup(line_buf);
        line->prev = NULL;
        line->next = NULL;
        line->text = duped_buf;
        if (buffer->current != NULL) {
            line->prev = buffer->current;
        }
        if (buffer->head == NULL) {
            buffer->head = line;
        }
        buffer->current = line;
    }

    char command_buf[1024];
    while (1) {
        fgets(command_buf, sizeof(command_buf), stdin);
        argument_parser(command_buf, buffer);
    }
}
