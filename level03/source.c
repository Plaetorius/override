int decrypt(char c)
{
  unsigned int len_secret_pass; 
  char secret_pass[29]; 

  secret_pass[17] = __readgsdword(0x14u);
  strcpy(secret_pass, "Q}|u`sfg~sf{}|a3");
  len_secret_pass = strlen(secret_pass);
  for (unsigned int i = 0; i < len_secret_pass; ++i )
    secret_pass[i] ^= c;
  if ( !strcmp(secret_pass, "Congratulations!") )
    return system("/bin/sh");
  else
    return puts("\nInvalid Password");
}

int test(int nb1, int nb2)
{
  int result; 
  char random;

  switch ( nb2 - nb1 )
  {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
      result = decrypt(nb2 - nb1);
      break;
    default:
      random = rand();
      result = decrypt(random);
      break;
  }
  return result;
}

int main()
{
	unsigned int seed;
	int input;

	seed = time(0);
	srand(seed);
	puts("***********************************");
	puts("*\t\tlevel03\t\t**");
	puts("***********************************");
	printf("Password:");
	scanf("%d", &input);
	test(input, 322424845);
	return 0;
}