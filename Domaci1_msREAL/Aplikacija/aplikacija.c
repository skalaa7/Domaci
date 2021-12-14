#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
	FILE* fp;
	size_t num_of_bytes = 200;
	char *option;
	
	while(1)
	{
		printf("1: Pročitaj trenutno stanje stringa\n");
		printf("2: Upiši novi string\n");
		printf("3: Konkataniraj string na trenutni\n");
		printf("4: Izbriši čitav string\n");
		printf("5: Izbriši vodeće i prateće space karaktere (Ponekad je potrebno vise puta pokrenuti da izbrise sve space karaktere)\n");
		printf("6: Izbriši izraz iz stringa\n");
		printf("7: Izbriši poslednjih n karaktera iz stringa\n");
		printf("Za izlaz iz aplikacije unesite Q\n");
		
		size_t nbytes = 1;
		option = (char *) malloc(nbytes+1);
		if(option==NULL)
		{
			perror("Problem pri alociranju option\n");
			return -1;
		}
		getline(&option,&nbytes, stdin);
		
		char *str;
		
		switch(*option)
		{
			case '1'://read
				fp = fopen ("/dev/stred", "r");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred\n");
					return -1;
				}
				str=(char *)malloc(num_of_bytes+1);
				getdelim(&str,&num_of_bytes,0,fp);
				printf("%s\n",str);	
				free(str);
				if(fclose(fp))
				{
					puts("Problem pri zatvaranju /dev/stred\n");
					return -1;
				}
				break;
			case '2'://string=
				fp = fopen ("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred\n");
					return -1;
				}
				printf("Unesite novi string: \n");
				str=(char *)malloc(num_of_bytes+1);
				getline(&str,&num_of_bytes,stdin);
				fprintf(fp,"string=%s",str);
				free(str);
				if(fclose(fp))
				{
					puts("Problem pri zatvaranju /dev/stred\n");
					return -1;
				}
				break;	
			case '3'://append=
				fp = fopen ("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred\n");
					return -1;
				}
				printf("Unesite string koji zelite da dodate na kraj: \n");
				str=(char *)malloc(num_of_bytes+1);
				getline(&str,&num_of_bytes,stdin);
				fprintf(fp,"append=%s",str);
				free(str);
				if(fclose(fp))
				{
					puts("Problem pri zatvaranju /dev/stred\n");
					return -1;
				}
				break;
			case '4'://clear
				fp = fopen ("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred\n");
					return -1;
				}
				fputs("clear\n",fp);
				if(fclose(fp))
				{
					puts("Problem pri zatvaranju /dev/stred\n");
					return -1;
				}
				break;
			case '5'://shrink
				fp = fopen ("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred\n");
					return -1;
				}
				fputs("shrink\n",fp);
				if(fclose(fp))
				{
					puts("Problem pri zatvaranju /dev/stred\n");
					return -1;
				}
				break;
			case '6'://remove=
				fp = fopen ("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred\n");
					return -1;
				}
				printf("Unesite izraz koji treba izbrisati: \n");
				str=(char *)malloc(num_of_bytes+1);
				getline(&str,&num_of_bytes,stdin);
				fprintf(fp,"remove=%s",str);
				free(str);
				if(fclose(fp))
				{
					puts("Problem pri zatvaranju /dev/stred\n");
					return -1;
				}
				break;
			case '7'://truncate
				fp = fopen ("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred\n");
					return -1;
				}
				printf("Koliko karaktera treba izbrisati? \n");
				str=(char *)malloc(num_of_bytes+1);
				getline(&str,&num_of_bytes,stdin);
				fprintf(fp,"truncate=%s",str);
				free(str);
				if(fclose(fp))
				{
					puts("Problem pri zatvaranju /dev/stred\n");
					return -1;
				}
				break;
			case 'Q':
				printf("Kraj aplikacije\n");
				return 0;
			default:
				break;
		}
		free(option);
	}
	return 0;
}
