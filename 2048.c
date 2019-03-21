/*projekt na jezyk C, Grzegorz Koziol*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

/*prototypy funkcji*/
void clear_screen();
int load_from_file(int **array);
int is_correct(int **array);
int game_logic(int **array,int *skip);
int check_win(int **array,int *win);
void generate(int **array);
void display_screen(int **array);
int check_lose(int **array);
int fill_zeros(int **array, char c);
int move_board(int **array,char c);
void save_to_file(int **array);
void display_help();

/*zmienne globalne*/
char line[50],c;
int i,j,k;
FILE *file;

int main()
{
    int f,**board;

    /*zmienna pozwalajaca pominac generowanie liczby do planszy, gdy jest rozna od zera*/
    int skip=1;

    srand(time(NULL));

    /*stworzenie tablicy 2d do gry*/
        	board = (int**)malloc(4* sizeof(int *));
        		if(board==NULL)
        		{
        			printf("Za malo pamieci");
        			exit(5);
        		}
        	for (i=0; i<4; i++)
        		{
        		board[i] = (int*)malloc(4* sizeof(int));
        		if(board[i]==NULL)
        		{
        			printf("Za malo pamieci");
        			exit(5);
        		}
        	}

    clear_screen();
	printf("Witaj w 2048. Chcesz wczytac plansze z pliku board.txt? Wpisz t(tak) lub n(nie): ");
    fgets(line,sizeof(line),stdin);
    while(!(sscanf(line,"%c",&c)==1 && (c=='n' || c=='t') && line[1]=='\n'))
    	{
    	printf(" Wpisz t(tak) lub n(nie): ");
    	fgets(line,sizeof(line),stdin);
    	}

    switch(c)
    {
        case 'n':
        /*wyzerowanie tablicy*/
        	for(i=0;i<4;i++)
        	{
            	for(j=0;j<4;j++)
            	{
                	board[i][j]=0;
            	}
        	}
        	break;

        case 't':
            skip=load_from_file(board);
			break;
    }

    f=game_logic(board,&skip);

    /*tu zapisywanie do pliku*/
    if(f==1)
    save_to_file(board);
    else printf("Przegrales. Nie ma juz zadnych dostepnych ruchow. Powodzenia nastepnym razem :)\n");

    /*czyszczenie tablic*/
    for(i=0;i<4;i++)
    free(board[i]);
    free(board);

    return 0;
}

/*czysci ekran*/
void clear_screen() {
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/*wczytuje plansze z pliku, nastepnie sprawdza jej poprawnosc*/
int load_from_file(int **array)
{
    int x;
    int skip=0;
    file=fopen("board.txt","r");
    if(file==NULL)
    {
        printf("Nie mozna otworzyc pliku board.txt\n");
        exit(1);
    }

    /*wczytywanie z pliku*/
        	for(i=0;i<4;i++)
        	{
        		fgets(line,sizeof(line),file);
        		x=sscanf(line,"%d %d %d %d",&array[i][0],&array[i][1],&array[i][2],&array[i][3]);
        		if(x!=4)
        		{
        			printf("Blad wczytywania planszy w linijce %d",i+1);
        			exit(2);
        		}
        	}
    fclose(file);
    skip=is_correct(array);
    return skip;
}

/*funkcja sprawdza poprawnosc wprowadzonej planszy.
jesli trafi na dozwolona wartosc, wychodzi z funkcji.
jesli nie znajdzie tej wartosci zglasza blad.
zwraca ilosc zer w planszy*/
int is_correct(int **array)
{

	int sum=0,zeros=0;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(array[i][j]==0) zeros+=1;
			else sum+=1;

            /*sprawdza potegi dwojki az do najwyzszej mozliwej wartosci w tej grze*/
			for(k=2;k<=131072;k*=2)
			{
				if(array[i][j]==k) break;
				else sum+=1;
			}
			if(sum==18)
			{
				printf("Nieprawidlowa wartosc bloku wiersz:%d kolumna:%d\n",i+1,j+1);
				exit(3);
			}
			sum=0;
		}
	}
	return zeros;
}

/*glowna funkcja programu,zawierajaca w sobie cala logike gry.
zwraca 1 jesli skonczyles gre albo wygrales, a 0 jesli przegrales*/
int game_logic(int **array,int *skip)
{
    int win=0,play=1,is_moved=0;
    char move;

    while(1)
    {
    /*sprawdzanie wygranej*/
    if(win==0) play=check_win(array,&win);
    else;

     if(play==0) return 1;

    /*jesli plansza nie jest pelna lub wykonano ruch, ktory cos
    zmienil, wtedy generuje liczbe na wolne miejsce w planszy*/
    if(*skip!=0) generate(array);
    else;

    display_screen(array);

    /*sprawdza czy przegrales*/
    play=check_lose(array);
     if(play==0) return 0;

    /*ruch gracza*/
    do{
		printf("Ruch:");
		fgets(line,sizeof(line),stdin);
		}while(!(sscanf(line,"%c",&move)==1 && (move=='w'||move=='W'||move=='s'||move=='S'||move=='a'||move=='A'||move=='d'||move=='D'||move=='q'||move=='p') && line[1]=='\n'));
		if(move=='p')
		{
		    display_help();
		    *skip=0;
		    continue;
		}
		if(move=='q')
		{
			return 1;
		}

	/*przesuwanie planszy*/
	is_moved+=fill_zeros(array,move);
	is_moved+=move_board(array,move);

	/*gdy nie wykonano zadnego ruchu planszy*/
	if(is_moved==0)
	{
	    *skip=0;
	    continue;
	}

    fill_zeros(array,move);
    is_moved=0;
    *skip=1;
    }/*koniec while*/
}

/*sprawdza czy doszedles do 2048.Nastepnie
pyta czy chcesz grac dalej. Jesli chcesz
to zwraca 1,jesli nie to zwraca 0. Jesli nie
znajdzie 2048 zwraca 1 i grasz dalej*/
int check_win(int **array,int *win)
{
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            if(array[i][j]==2048 && *win==0)
            {
                *win=1;
                printf("Gratulacje!!. Doszedles do 2048. Chcesz grac dalej ? Wpisz t(tak) lub n(nie): ");
                fgets(line,sizeof(line),stdin);
                while(!(sscanf(line,"%c",&c)==1 && (c=='t' || c=='n') && line[1]=='\n'))
                {
                    printf("Wpisz t(tak) lub n(nie): ");
                    fgets(line,sizeof(line),stdin);
                }

                if(c=='t') return 1;
                if(c=='n') return 0;
            }
        }
    }

    return 1;
}

/*generuje 2 lub 4 na wolne miejsce w planszy*/
void generate(int **array)
{
     do{
        i=rand()%4;
        j=rand()%4;
        k=rand()%10;
    }while(array[i][j]!=0);
    if(k<2)  array[i][j]=4;
    else     array[i][j]=2;
}

/*wyswietla plansze*/
void display_screen(int **array)
{
    clear_screen();
    printf("\n\t\t\t\t --Gra 2048--\n\n\n");

    for (i = 0; i<4; i++)
	{
		printf("\t\t     |");

		for (j = 0; j<4; j++)
		{
			if (array[i][j])
				printf("%4d    |", array[i][j]);
			else
				printf("%4c    |", ' ');
		}
		printf("\n");
    }
    printf("\n\n");

    printf("  w\t\t  ^\t\t\t p-pomoc\tq-koniec\n");
    printf("a s d\t\t< v >\n\n");
}

/*sprawdza czy sa dostepne ruchy patrzac na wszystkich sasiadow(nie po skosie).0 liczy sie jako para.
jesli znajdzie jakas pare to mozna wykonac ruch. jesli nie znajdzie pary to koniec gry*/
int check_lose(int **array)
{
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(i!=0)
			{
				if(array[i][j]==array[i-1][j] || array[i-1][j]==0) return 1;
			}

			if(i!=3)
			{
				if(array[i][j]==array[i+1][j] || array[i+1][j]==0) return 1;
			}

			if(j!=0)
			{
				if(array[i][j]==array[i][j-1] || array[i][j-1]==0) return 1;
			}

			if(j!=3)
			{
				if(array[i][j]==array[i][j+1] || array[i][j+1]==0) return 1;
			}
		}
	}
	/*gdy nie znajdzie sasiadow*/
	return 0;
}

/*przesuwa liczby w dana strone,aby nie bylo zer miedzy nimi.
zwraca 1 jesli wykonano jakas akcje przesuniecia*/
int fill_zeros(int **array, char c)
{
    int moves=0;

    switch(c)
    {
        case 'w':
        case 'W':
        for (i = 0; i<4; i++)
			for (j = 0; j<4; j++)
			{
				if (!array[j][i])
				{
					for (k = j + 1; k<4; k++)
						if (array[k][i])
						{
							array[j][i] = array[k][i];
							array[k][i] = 0;
							moves=1;
							break;
						}
				}
			}
			break;

	    case 's':
        case 'S':
        	for (i = 0; i<4; i++)
			for (j = 3; j >= 0; j--)
			{
				if (!array[j][i])
				{
					for (k = j - 1; k >= 0; k--)
						if (array[k][i])
						{
							array[j][i] = array[k][i];
							array[k][i] = 0;
							moves=1;
							break;
						}
				}
			}
	    	break;

	    case 'a':
        case 'A':
	    	for (i = 0; i<4; i++)
			for (j = 0; j<4; j++)
			{
				if (!array[i][j])
				{
					for (k = j + 1; k<4; k++)
						if (array[i][k])
						{
							array[i][j] = array[i][k];
							array[i][k] = 0;
							moves=1;
							break;
						}
				}
			}
		    break;

		case 'd':
        case 'D':
        for (i = 0; i<4; i++)
			for (j = 3; j >= 0; j--)
			{
				if (!array[i][j])
				{
					for (k = j - 1; k >= 0; k--)
						if (array[i][k])
						{
							array[i][j] = array[i][k];
							array[i][k] = 0;
							moves=1;
							break;
						}
				}
			}
			break;
    }

    return moves;
}

/*paruje te same liczby w jedna, a druga zamienia na zero
zwraca 1 jesli wykonano jakas akcje przesuniecia*/
int move_board(int **array,char c)
{
    int moves=0;

    switch(c)
    {
        case 'w':
        case 'W':
         for (i = 0; i<4; i++)
			for (j = 0; j<3; j++)
			{
				if (array[j][i] && array[j][i] == array[j + 1][i])
				{
					array[j][i] += array[j + 1][i];
					array[j + 1][i] = 0;
					moves=1;
				}
			}
        break;

        case 's':
        case 'S':
         for (i = 0; i<4; i++)
			for (j = 3; j>0; j--)
			{
				if (array[j][i] && array[j][i] == array[j - 1][i])
				{
					array[j][i] += array[j - 1][i];
					array[j - 1][i] = 0;
				    moves=1;
				}
			}
			break;

		case 'a':
        case 'A':
         for (i = 0; i<4; i++)
			for (j = 0; j<3; j++)
			{
				if (array[i][j] && array[i][j] == array[i][j + 1])
				{
					array[i][j] += array[i][j + 1];
					array[i][j + 1] = 0;
					moves=1;
				}
			}
			break;

		case 'd':
        case 'D':
         for (i = 0; i<4; i++)
			for (j = 3; j>0; j--)
			{
				if (array[i][j] && array[i][j] == array[i][j - 1])
				{
					array[i][j] += array[i][j - 1];
					array[i][j - 1] = 0;
					moves=1;
				}
			}
			break;
    }
    return moves;
}

/*funkcja zapisuje plansze do pliku jesli zechcesz*/
void save_to_file(int **array)
{
	clear_screen();
    printf("Czy chcesz zapisac wynik w pliku board.txt? Wpisz t(tak) lub n(nie): ");
			fgets(line,sizeof(line),stdin);
			while(!(sscanf(line,"%c",&c)==1 && (c=='n' || c=='t') && line[1]=='\n'))
			{
				printf("Wpisz t(tak) lub n(nie): ");
				fgets(line,sizeof(line),stdin);
			}
    switch(c)
    {
        case 't':
    	file=fopen("board.txt","w");
      	if(file==NULL)
    	{
    		printf("Nie mozna otworzyc pliku board.txt\n");
	    	exit(1);
    	}

    	for(i=0;i<4;i++)
    	{
    		fprintf(file,"%d %d %d %d\n",array[i][0],array[i][1],array[i][2],array[i][3]);
    	}
    	fclose(file);
    	break;

    	case 'n':
    	break;
    }
    printf("Dziekuje za gre. Do zobaczenia\n");
}

/*wyswietla instrukcje gry*/
void display_help()
{
    clear_screen();
	printf("Witaj w grze 2048!\n"
	"Zasady sa proste.Celem gry jest ulozenie na planszy na jednym polu wspomnianej liczby 2048.\n"
	"Aby wykonac ruch nalezy wcisnac jeden z klawiszy(w,s,a,d,p,q) + enter aby zatwierdzic.\n"
	"Nastepnie wszystkie klocki ktore sie pojawily na planszy przesuwaja sie w danym kierunku.\n"
	"W-w gore, S-w dol, A-w lewo, D-w prawo.\n"
	"Nastepnie wykonywane jest laczenie tych samych par liczb w jedna 2x wieksza.\n"
	"Przykladowo w jednym wierszu mamy 2-0-2-0 i przesuwamy w prawo. Wtedy otrzymamy 0-0-0-4\n");
	printf("Inny przyklad: mamy 2-2-2-2 to przesuwajac w lewo bedziemy mieli 4-4-0-0.\n"
	"Dobra strategia jest trzymanie swojej najwiekszej liczby w ktoryms z rogow.\n"
	"Najwieksza cyfra jaka mozesz uzyskac w bloku to 131072 tak wiec powodzenia.\n"
	"Wcisnij jakis klawisz,aby kontynuowac: ");
	fgets(line,sizeof(line),stdin);
}