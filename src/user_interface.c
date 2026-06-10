#include "global.h"
#include "errors.h"
#include "net_graph.h"
#include "networking_logic.h"
#include "neighbourhood_creep.h"


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

/**************************/
/*********PROGRAM**********/
/**************************/

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

/**
 * Donne le nombre de tick.
 */
size_t get_global_tick()
{
    return program.tick_counter;
}

/**************************/
/**********INPUTS**********/
/**************************/

#define LINE_SIZE 1250
#define BUFFER_SIZE 64
char line[LINE_SIZE];
char first[BUFFER_SIZE], second[BUFFER_SIZE], third[BUFFER_SIZE], fourth[BUFFER_SIZE], fifth[BUFFER_SIZE];

/**
 * Recois les données écrites par l'utilisateur.
 * 
 * Retourne : Le nombre de caractére prise.
 */
static int get_inputs()
{
    memset(line,  0, sizeof(line)  );
    memset(first, 0, sizeof(first) );
    memset(second,0, sizeof(second));
    memset(third, 0, sizeof(third) );
    memset(fourth,0, sizeof(fourth));
    memset(fifth, 0, sizeof(fourth));

    // Pour un meileur affichage input
    fprintf(stderr, ">>$ ");

    // fgets c'est trop bien
    if (fgets(line, sizeof(line), stdin) == NULL) 
    {
        fprintf(stderr, "Read error\n");
        return ERROR_COULDNT_READ;
    }

    int count = sscanf(line, "%63s %63s %63s %63s %63s", first, second, third, fourth, fifth);

    first[BUFFER_SIZE-1]   = '\0';
    second[BUFFER_SIZE-1]  = '\0';
    third[BUFFER_SIZE-1]   = '\0';
    fourth[BUFFER_SIZE-1]  = '\0';
    fifth[BUFFER_SIZE-1]  = '\0';

    return count;
}

/**************************/
/********COMMANDES*********/
/**************************/

const char* INSTRUCTIONS = 
    "\tbp       : Sets a new breakpoint frequency.\n \t\tEx: bp 1000\n"
    "\tcontinue : Continues the program.\n"
    "\texit     : Exits the program.\n"
    "\thelp     : Show these informations.\n"
    "\tdisplay  : Show all the nodes and devices in the network\n";

/* Commandes pour l'utilisateur */
static void c_help() {fprintf(stderr, "%s", INSTRUCTIONS);}
static void c_continue() {continue_program(true);}
static void c_bp() 
{
    size_t new_freq = 1;
    if(sscanf(second, "%zu", &new_freq) != 1)
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
static void c_exit() 
{
    fprintf(stderr, "Exiting program\n");
    close_program();
}
static void c_display(struct net_graph* graph) 
{
    enum EXIT_CODES ex = network_display(graph);
    if(ex != SUCESS)
    {
        DEBUG_WARNING(ex);
        fprintf(stderr, "Error while displaying network.\n");
    }
}
static void c_send(struct net_graph* graph, struct neighbourhood* nei)
{
    fprintf(stderr, "Hello\n");

    size_t idx1 = INVALID_INDEX, idx2 = INVALID_INDEX;

    if(sscanf(second,"%zu", &idx1) != 1)
    {
        fprintf(stderr, "Error: argument 1 must be an integer\n");
        return;
    }
    if(sscanf(third, "%zu", &idx2) != 1)
    {
        fprintf(stderr, "Error: argument 2 must be an integer\n");
        return;
    }

    if(idx1 >= graph->nb_devices || idx2 >= graph->nb_devices)
    {
        fprintf(stderr, "Error: indexes must be bewteen 0 and %zu\n", graph->nb_devices);
        return;
    }


    size_t size = strlen(fourth);
    user_queues_packet(graph, nei, idx1, idx2, fourth, size, 20);
}

/****Fonctions d'aides****/

static bool is_equal_str(const char* input, const char* command)
{
    return strcmp(input, command) == 0;
}

static void invalid_command_message()
{
    fprintf(stderr, "Error : invalid command or command arguments : '%s' \n", first);
}

static bool invalid_args_message(const char* string, int args, int expected)
{
    if(args != expected)
    {
        fprintf(stderr, "Error : '%s' waited for %d arguments\n", string, expected);
        return true;
    }
    return false;
}

static void parse_commands(int count, struct net_graph* graph, struct neighbourhood * neighbourhood)
{
    // Attends 1 argument
    if(is_equal_str(first, "help"))         
    {
        if(invalid_args_message("bp", count, 1))
            return;
        c_help();
    }
    else if(is_equal_str(first, "continue"))
    {
        if(invalid_args_message("bp", count, 1))
            return;
        c_continue();
    }
    else if(is_equal_str(first, "exit"))
    {    
        if(invalid_args_message("bp", count, 1))
            return;
        c_exit();
    }
    else if(is_equal_str(first, "display"))
    { 
        if(invalid_args_message("bp", count, 1))
            return;
        c_display(graph);
    }

    // Attends 2 arguments
    else if(is_equal_str(first, "bp")) 
    {
        if(invalid_args_message("bp", count, 2))
            return;
        c_bp();
    }

    // Attends 4 arguments
    else if(is_equal_str(first, "send"))
    { 
        if(invalid_args_message("send", count, 4))
            return;
        c_send(graph, neighbourhood);
    }

    // Invalid
    else invalid_command_message();
}

/**************************/
/*********INFO_TEXT*********/
/**************************/

static void display_infos()
{
    printf(WELCOME_TEXT);
    printf("\nCommands : \n");
    printf(INSTRUCTIONS);
}

/**************************/
/**************************/
/**************************/

/**
 * Interface terminal pour l'utilisateur.
 * 
 * Listes des commandes possibles:
 * 
 *      - bp : Sets a new breakpoint frequency. Ex: bp 1000
 *      - continue : Continues the program.
 *      - exit     : Exits the program.
 *      - help     : Show these informations.
 *      - display  : Show all the nodes and devices in the network
 */
enum EXIT_CODES user_loop(struct net_graph graph, struct neighbourhood * neighbourhood)
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

                parse_commands(count, &graph, neighbourhood);
            }
        }

        network_handle_tick(&graph, neighbourhood);
        increase_tick();
    }

    return ex_code;
}