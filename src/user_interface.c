#include "global.h"
#include "errors.h"
#include "net_graph.h"
#include "networking_logic.h"

#include <stdio.h>
#include <string.h>

/* Couleurs ANSI */
#define ANSI_RESET      "\x1b[0m"
#define ANSI_BOLD       "\x1b[1m"

// Au debut, il y'a tant de tours avant un breakpoint
#define TICK_BEFORE_BP 50  

#define ANSI_GREEN      "\x1b[38;5;82m"
#define ANSI_YELLOW       "\x1b[38;5;226m"

const char* WELCOME_TEXT = ANSI_BOLD "Copyright " G_YEAR " IUT Robert Schuman (BUT) " ANSI_RESET "\n" \
G_APP_NAME " " G_VERSION "\n" \
"This is free software: you are free to change and redistribute it.\n" \
"There is NO WARRANTY, to the extent permitted by law.\n"
"For bug reporting instructions, please see: " \
ANSI_GREEN "https://git.unistra.fr/vasilescu/saesystem/-/issues" ANSI_RESET "\n";


/*********PROGRAM***********/

#define BP_MAX 100000

/* Les infos sur le programme */
struct program {
    bool program_should_close;
    size_t breakpoint_freq;
    size_t tick_counter;
    bool continue_prog;
};
// Initalisation statique
static struct program program = {false, TICK_BEFORE_BP, 0, true};

static bool program_should_close()
{
    return program.program_should_close;
}
static void increase_tick()
{
    program.tick_counter++;
}
static bool time_to_stop()
{
    return (program.tick_counter % program.breakpoint_freq) == 0;
}
static void set_new_breakpoint_freq(size_t new)
{
    program.breakpoint_freq = new;
}
static bool program_continues()
{
    return program.continue_prog;
}
static void continue_program(bool stop)
{
    program.continue_prog = stop;
}
static void close_program()
{
    program.program_should_close = true;
}

/**********INPUTS**********/

#define LINE_SIZE 130
#define BUFFER_SIZE 64
char line[LINE_SIZE];
char first[BUFFER_SIZE], last[BUFFER_SIZE];

/**
 * Recois les données écrites par l'utilisateur.
 * 
 * Retourne : Le nombre de caractére prise.
 */
static int get_inputs()
{
    memset(line,  0, sizeof(line) );
    memset(first, 0, sizeof(first));
    memset(last,  0, sizeof(last) );

    fprintf(stderr, ">>$ ");

    // fgets c'est trop bien
    if (fgets(line, sizeof(line), stdin) == NULL) 
    {
        fprintf(stderr, "Read error\n");
        return ERROR_COULDNT_READ;
    }

    int count = sscanf(line, "%63s %63s", first, last);

    first[BUFFER_SIZE-1] = '\0';
    last[BUFFER_SIZE-1]  = '\0';

    return count;
}

/*****COMMANDES*****/
const char* INSTRUCTIONS = 
    "\tbp       : Sets a new breakpoint frequency.\n \t\tEx: bp 1000\n"
    "\tcontinue : Continues the program.\n"
    "\texit     : Exits the program.\n"
    "\thelp     : Show these informations.\n"
    "\tdisplay  : Show all the nodes and devices in the network\n";

static void parse_two()
{
    /**
     * bp : Sets a new breakpoint frequency.
     */
    if(strcmp(first, "bp") == 0)
    {
        size_t new_freq = 1;
        if(sscanf(last, "%zu", &new_freq) != 1)
        {
            fprintf(stderr, "Error: Invalid arguments, expected an integer value\n");
            return;
        }
        if(new_freq < 1 || new_freq > BP_MAX)
        {
            fprintf(stderr, "Error: the new breakpoint frequence should be between 1 and %zu\n", (size_t)BP_MAX);
            return;
        }
        set_new_breakpoint_freq(new_freq);
        fprintf(stderr, "New bp has been set.\n");
    }
    else
    {
        fprintf(stderr, "Error : invalid command '%s' \n", first);
    }
}

/*********INFO_TEXT*********/

static void display_infos()
{
    printf(WELCOME_TEXT);
    printf("\nCommands : \n");
    printf(INSTRUCTIONS);
}


void parse_one(struct net_graph* graph)
{
    if(strcmp(first, "help") == 0)
    {
        fprintf(stderr, "%s", INSTRUCTIONS);
    }
    else if(strcmp(first, "continue") == 0)
    {
        continue_program(true);
    }
    else if(strcmp(first, "exit") == 0)
    {
        fprintf(stderr, "Exiting program\n");
        close_program();
    }
    else if(strcmp(first, "display") == 0)
    {
        enum EXIT_CODES ex = network_display(graph);
        if(ex != SUCESS)
        {
            DEBUG_WARNING(ex);
            fprintf(stderr, "Erreur dans l'affichage du réseau.\n");
        }
    }
    else
    {
        fprintf(stderr, "Error : invalid command '%s' \n", first);
    }
}

static void parse_command(int count, struct net_graph* graph)
{
    if (count < 1) fprintf(stderr, "Error: expected at least one word\n");
    else if (count == 1) parse_one(graph);
    else parse_two();
}

/*******************/

/**
 * Interace terminal pour l'utilisateur.
 * 
 * Listes des commandes possibles:
 * 
 *      - bp : Sets a new breakpoint frequency. Ex: bp 1000
 */
enum EXIT_CODES user_loop(struct net_graph graph)
{
    enum EXIT_CODES ex_code = SUCESS;
    display_infos();

    while(!program_should_close())
    {
        if(time_to_stop())
        {
            fprintf(stderr, "%s Current ticks : %zu\n %s \n", ANSI_YELLOW, program.tick_counter, ANSI_RESET);
            continue_program(false);
            while(!program_continues() && !program_should_close())
            {
                // Les ajoutes dans les buffers
                int count = get_inputs();

                parse_command(count, &graph);
            }
        }


        increase_tick();
    }

    return ex_code;
}