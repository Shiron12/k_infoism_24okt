#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

extern int errno;

#define RED "\e[91m"
#define RESET "\x1B[0m"
#define error_MSG fprintf(stderr, "%sIn file: %s%s%s and in function: %s%s%s line: %s%d%s an error occured.\nERROR MESSAGE: %s%s%s\nERRNO CODE: %s%d%s\n", RESET, RED, __FILE__, RESET, RED, __func__, RESET, RED, __LINE__, RESET, RED, strerror(errno), RESET, RED, errno, RESET);
#define STR_MAX_LEN 256

typedef struct data_block
{
    int azon;
    char nev[STR_MAX_LEN];
    int hossz;
    int melyseg;
    char telepules[STR_MAX_LEN];
    enum
    {
        FOKOZOTTAN_VEDETT,
        MEGKULOMBOZTETETTEN_VEDETT,
        VEDETT
    }vedettseg;

    struct data_block *next;
} DATA;

DATA* add(DATA *curr, DATA tmp)
{
    DATA *link = malloc(sizeof (DATA));
    
    if(link == NULL)
    {
        error_MSG;
        exit(errno);
    }

    link->azon = tmp.azon;
    strcpy(link->nev, tmp.nev);
    link->hossz = tmp.hossz;
    link->melyseg = tmp.melyseg;
    strcpy(link->telepules, tmp.telepules);
    link->vedettseg = tmp.vedettseg;

    link->next = NULL;
    
    if(curr != NULL)
        curr->next = link;

    return link;
}

void del(DATA *head)
{
    DATA *curr = head;
    DATA *tmp = head->next;

    while(tmp != NULL)
    {
        free(curr);
        curr = tmp;
        tmp = tmp->next;
    }
}

int getVedettseg (char *fokozat)
{
    if(!strcmp(fokozat, "fokozottan védett"))
        return FOKOZOTTAN_VEDETT;
    else if(!strcmp(fokozat, "védett"))
        return VEDETT;
    else if(!strcmp(fokozat, "megkülönböztetetten védett"))
        return MEGKULOMBOZTETETTEN_VEDETT;
    else
        return -1;
}

void printDATA(DATA * head, int index)
{
    int k = 0;
    for(DATA *i = head; i != NULL; i = i->next, k++)
    {
        if(k == index)
        {
            printf("\tAzon: %d\n\tNev: %s\n\tHossz: %d m\n\tMelyseg: %d m\n\tTelepules: %s\n", i->azon, i->nev, i->hossz, i->melyseg, i->telepules);
            return;
        }
    }
}

int main(int argc, char **argv)
{
    FILE *file = fopen("barlangok.txt", "r");

    if(file == NULL)
    {
        error_MSG;
        exit(errno);
    }

    int skip = 1;
    int DATA_SIZE = 0;
    int MISKOLC_DB = 0;

    int INDEX_v = 0;
    int INDEX_fv = 0;
    int INDEX_mv = 0;

    int MAX_v = -1;
    int MAX_fv = -1;
    int MAX_mv = -1;

    int STAT_v = 0;
    int STAT_fv = 0;
    int STAT_mv = 0;

    double SUM_MISKOLC = 0.0;

    char tmp[STR_MAX_LEN];
    char sub[STR_MAX_LEN];

    DATA *head = NULL, *current = NULL;
    DATA d;

    while(fgets(tmp, STR_MAX_LEN, file) != NULL)
    {
        if(skip)
        {
            skip = 0;
            continue;
        }

        //somebody teach me fscanf(file, "%*[^@] %[^;]", tmp); <----- how to do %*[^;] i cant find on internet ^_^

        for(int i = 0, start = 0, statement = 0; tmp[i] != '\n' && tmp[i] != 0; i++)
        {
            if(tmp[i] == ';' || tmp[i] == '\r' || tmp[i] == '\n' || tmp[i] == 0)
            {
                int k = 0;
                for(; start != i; k++, start++)
                    sub[k] = tmp[start];
                sub[k++] = 0;
                start = i + 1;
                statement++;

                switch(statement)
                {
                    case 1: d.azon = atoi(sub); break; 
                    case 2: strcpy(d.nev, sub); break;
                    case 3: d.hossz = atoi(sub); break;
                    case 4: d.melyseg = atoi(sub); break;
                    case 5: 

                        strcpy(d.telepules, sub);
                        int j = 0;

                        for(; d.telepules[j] != ' ' && d.telepules[j] != 0; j++)
                            tmp[j] = d.telepules[j];
                        
                        tmp[j] = 0;

                        if(!strcmp(tmp, "Miskolc"))
                        {
                            SUM_MISKOLC += d.melyseg;
                            MISKOLC_DB++;
                        }

                    break;
                    case 6: 

                        d.vedettseg = getVedettseg(sub);

                        //pre calculate 6. feladat and 7. feladat
                        switch(d.vedettseg)
                        {
                            case FOKOZOTTAN_VEDETT: if(MAX_fv < d.hossz){MAX_fv = d.hossz; INDEX_fv = DATA_SIZE;} STAT_fv++; break;
                            case VEDETT: if(MAX_v < d.hossz){MAX_v = d.hossz; INDEX_v = DATA_SIZE;} STAT_v++; break;
                            case MEGKULOMBOZTETETTEN_VEDETT: if(MAX_mv < d.hossz){MAX_mv = d.hossz; INDEX_mv = DATA_SIZE;} STAT_mv++; break;
                        }
                        
                    break;
                }
            }
        }

        if(head == NULL)
        {
            head = add(head, d);
            current = head;
        }
        else
            current = add(current, d);

        DATA_SIZE++;
    }
    
    printf("4. feladat: Barlangok szama: %d\n", DATA_SIZE);
    printf("5. feladat: Az atlagos melyseg: %.3f m\n", SUM_MISKOLC / MISKOLC_DB);
    printf("6. feladat: Kerem a vedettsegi szintet: ");
    
    if(fgets(tmp, STR_MAX_LEN, stdin) == NULL)
    {
        error_MSG;
    }

    for(int i = 0; tmp[i] != 0; i++)
    {
        if(tmp[i] == '\n')
        {
            tmp[i] = 0;
            break;
        }
    }

    switch(getVedettseg(tmp))
    {
        case FOKOZOTTAN_VEDETT: printDATA(head, INDEX_fv); break;
        case VEDETT: printDATA(head, INDEX_v); break;
        case MEGKULOMBOZTETETTEN_VEDETT: printDATA(head, INDEX_mv); break;
        default : printf("\tNincs ilyen védettségi szinttel barlang az adatok között!\n"); break;
    }

    printf("7. feladat: Statisztika\n\tfokozottan vedett:------------> %d db\n\tmegkulomboztetetten vedett:---> %d db\n\tvedett:-----------------------> %d db\n", STAT_fv, STAT_mv, STAT_v);

    if(DATA_SIZE <= 1)
        free(head);
    else
        del(head);

    free(current);
    fclose(file);
    return 0;
}