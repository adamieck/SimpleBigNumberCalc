#define N 1048576
#define FNSIZE 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _BigInt 
{
	int* digits; // int vector
	size_t size; // num of elements in a vector
} BigInt;

char to_char(int a) // convert int to char
{
	if (a >= 0 && a <= 9) return a + '0';
	else if ((a >= 10 && a <= 16)) return a + 'A' - 10;
}
int to_num(char c) // convert char to int
{
	if (c >= '0' && c <= '9')   return c - '0';
	else if ((c >= 'A' && c <= 'F')) return c - 'A' + 10;
}

void str_to_bigint(BigInt* a, char* s) // convert string to BigInt
{
	size_t str_size;
	size_t k;

	str_size = strlen(s);
	a->digits = (int*)malloc((str_size - 1) * sizeof(int));
	if (a->digits == NULL)
	{	
		return;
	}
	a->size = str_size - 1;
	

	k = 0;
	for (long long i = str_size - 1; i >= 0; i--)
	{
		if (s[i] != '\n')
		{
			*(a->digits + k) = to_num(s[i]);
			k++;
		}
	}
}

void bigint_zero(BigInt* a, size_t n) // allocate memory and fill it with 0s
{

	a->digits = calloc(n, sizeof(int));
	if (a->digits == NULL) 
		printf("Calloc failed.");
	a->size = n;
}

void bigint_realloc(BigInt* a, size_t k) // reallocate memory
{
	size_t n;
	n = a->size;
	if (k > 0)
	{
		int* tmp = (int*)realloc(a->digits, k * sizeof(int));
		if (tmp != NULL)
		{
			a->digits = tmp;
			a->size = k;
		}
	}
	if (k > n) // if memory block is expanded, fill new space with 0s
	{
		for (size_t i = n; i < a->size; i++)
			a->digits[i] = 0;
	}
}

void bigint_strip(BigInt* a) // remove excessive 0s
{
	size_t k;
	size_t n;

	n = a->size;
	k = 0;

	for (long long i = n - 1; i > 0; i--)
	{
		if (a->digits[i] == 0) k++;
		else break;
	}
	if (k > 0) bigint_realloc(a, n - k);

}

void print_bigint(BigInt* a) // print BigInt
{
	for(long long i = a->size - 1; i >= 0; i--)
		printf("%c", to_char(a->digits[i]));
	printf("\n");
}

void bigint_fprint(BigInt* a, FILE* output) // print BigInt to file
{
	for (long long i = a->size - 1; i >= 0; i--)
		fprintf(output, "%c", to_char(a->digits[i]));
	fprintf(output, "\n");
}

void bigint_free(BigInt* a) // free memory
{
	free(a->digits);
}

void add(BigInt a, BigInt b, BigInt* result, int base) // add BigInts
{
	int carry, sum;
	size_t n;

	n = (a.size > b.size) ? a.size : b.size; n++;
	bigint_zero(result, n);


	carry = 0;
	for (unsigned long long i = 0; i < n; i++)
	{
		sum = (i < a.size ? a.digits[i] : 0) + (i < b.size ? b.digits[i] : 0) + carry;

		
		if (sum >= base)
		{
			carry = 1;
			sum -= base;
		}
		else carry = 0;

		result->digits[i] = sum;
	}
	bigint_strip(result);
}

void multiply(BigInt a, BigInt b, BigInt* result, int base) // multiply BigInts
{
	int r, w;
	size_t na, nb, n;
	na = a.size;
	nb = b.size;
	n = na > nb ? na : nb;

	bigint_zero(result, n * 2);

	for (size_t i = 0; i < nb; i++)
	{
		r = 0;
		for (size_t j = 0; j < na; j++) {
			w = a.digits[j] * b.digits[i] + r + result->digits[i + j];
			result->digits[i + j] = w % base;
			r = w / base;
		}
		result->digits[na + i] += r;
	}
	bigint_strip(result);
}

void subtract(BigInt a, BigInt b, BigInt* result, int base) // used for division (assuming a > b)
{
	int dif;
	size_t n;
	
	n = a.size;
	bigint_zero(result, n);

	for (size_t i = 0; i < n; i++)
	{
		dif = a.digits[i] - (i < b.size ? b.digits[i] : 0);
		if (dif < 0)
		{
			if (i + 1 < n) a.digits[i + 1]--;
			dif += base;
		}
		result->digits[i] = dif;
	}
	bigint_free(&a);
}


int bigint_lesser(BigInt a, BigInt b) // returns 1 if a<b
{
	int lesser, equal, ai, bi;
	size_t n, na, nb;

	lesser = 0;
	equal = 1;
	na = a.size;
	nb = b.size;
	n = a.size > b.size ? a.size : b.size;
	for (long long i = n-1; i >= 0; i--)
	{
		ai = i < na ? a.digits[i] : 0;
		bi = i < nb ? b.digits[i] : 0;
		if (ai < bi)
		{
			lesser = 1;
			break;
		}
		else
		{
			if (bi < ai)
			{
				lesser = 0;
				break;
			}
		}
	}

	return lesser;
}

int bigint_equal(BigInt a, BigInt b) // returns 1 if a==b
{
	int equal, ai, bi;
	size_t n, na, nb;

	equal = 1;
	na = a.size;
	nb = b.size;
	n = a.size > b.size ? a.size : b.size;

	for (long long i = n - 1; i >= 0; i--)
	{
		ai = i < na ? a.digits[i] : 0;
		bi = i < nb ? b.digits[i] : 0;
		if (ai != bi)
		{
			equal = 0;
			break;
		}
	}
	return equal;
}

int bigint_greater(BigInt a, BigInt b) // returns 1 if a > b
{
	return !(bigint_lesser(a, b) || bigint_equal(a, b));
}

int is_zero(BigInt a) // returns 1 if a == 0
{
	int z = 1;
	size_t n;
	n = a.size;
	
	for (long long i = 0; i < n; i++)
	{
		if (a.digits[i] != 0)
		{
			z = 0;
			break;
		}
	}
	return z;
}

BigInt divide(BigInt a, BigInt b, BigInt* result, int base) // divide BigInts
{
	BigInt rem;
	size_t n;
	n = a.size;
	bigint_zero(result, n);
	bigint_zero(&rem, n);
	if (n > 1)
	{
		for (long long i = n - 1; i >= 0; i--)
		{
			for (long long j = n - 2; j >= 0; j--)
				rem.digits[j + 1] = rem.digits[j];
			rem.digits[0] = a.digits[i];
			while (bigint_lesser(b, rem) || bigint_equal(b, rem))
			{
				++(result->digits[i]);
				subtract(rem, b, &rem, base);
			}
		}
	}
	else {
		if (bigint_lesser(a, b))
		{
			result->digits[0] = 0;
			rem.digits[0] = a.digits[0];
		}
		else if (bigint_equal(a, b))
		{
			result->digits[0] = 1;
			rem.digits[0] = 0;
		}
		else
		{
			result->digits[0] = a.digits[0] / b.digits[0];
			rem.digits[0] = a.digits[0] % b.digits[0];
		}
	}
	bigint_strip(result);
	bigint_strip(&rem);
	return rem; // return remainder
}

BigInt remainder(BigInt a, BigInt b, int base) // calculate a % b
{
	BigInt t;
	return (divide(a, b, &t, base));
}

BigInt power(BigInt a, BigInt b, int base) // calculate a^b
{
	BigInt zero, two, w;

	bigint_zero(&zero, 1);
	bigint_zero(&two, 1);
	bigint_zero(&w, 1);
	two.digits[0] = 2; 
	w.digits[0] = 1;

	while (bigint_greater(b, zero)) // while b > 0
	{
		if (b.digits[0] % 2 == 1) multiply(w, a, &w, base);
		multiply(a, a, &a, base);
		divide(b, two, &b, base);
	}
	bigint_free(&zero);
	bigint_free(&two);
	bigint_strip(&w);
	return w;
}


int big_to_int(BigInt a) // used for conversion
{
	int w, p;
	size_t n;
	n = a.size;
	w = 0;
	p = 1;

	for (size_t i = 0; i < n; i++)
	{
		w += a.digits[i] * p;
		p *= 10;
	}
	return w;
}
BigInt any_to_dec(BigInt a, int base) // any base to decimal
{
	BigInt w, base_big, num;
	size_t n;

	n = a.size;

	if (base >= 10)
	{
		bigint_zero(&w, 2);
		bigint_zero(&num, 2);
		bigint_zero(&base_big, 2);
		base_big.digits[0] = base % 10;
		base_big.digits[1] = (base / 10) % 10;
		w.digits[0] = a.digits[n - 1] % 10;
		w.digits[1] = (a.digits[n - 1] / 10) % 10;
	}
	else
	{
		bigint_zero(&w, 1);
		bigint_zero(&num, 1);
		bigint_zero(&base_big, 1);
		base_big.digits[0] = base;
		w.digits[0] = a.digits[n - 1];
	}
	if (n > 1)
	{
		for (long long i = n - 2; i >= 0; i--)
		{
			if (base >= 10)
			{
				num.digits[0] = a.digits[i] % 10;
				num.digits[1] = (a.digits[i] / 10) % 10;
			}
			else num.digits[0] = a.digits[i];
			multiply(w, base_big, &w, 10);
			add(w, num, &w, 10);
		}
	}
	else
	{
		bigint_free(&base_big);
		bigint_free(&num);
		return w;
	}
	bigint_free(&base_big);
	bigint_free(&num);
	return w;
}

BigInt dec_to_any(BigInt a, int to_base) // decimal to any base 
{
	BigInt w, zero, base_big;
	size_t i;

	bigint_zero(&w, 1);
	bigint_zero(&zero, 1);

	if (to_base >= 10)
	{
		bigint_zero(&base_big, 2);
		base_big.digits[0] = to_base % 10;
		base_big.digits[1] = (to_base / 10) % 10;
	}
	else
	{
		bigint_zero(&base_big, 1);
		base_big.digits[0] = to_base;
	}

	i = 0;
	while (bigint_greater(a,zero)) // while a > 0
	{
		w.digits[i] = big_to_int(divide(a, base_big, &a, 10));
		bigint_realloc(&w, w.size + 1);
		i++;
	}
	bigint_free(&base_big);
	bigint_free(&zero);
	bigint_strip(&w);
	return w;
}

BigInt convert(BigInt a, int base, int to_base) // convert any base to any base
{
	BigInt w;
	if (base == to_base) return a;
	else if (base == 10) w = dec_to_any(a, to_base);
	else if (to_base == 10) w = any_to_dec(a, base);
	else
	{
		w = any_to_dec(a, base);
		w = dec_to_any(w, to_base);
	}
	return w;
}

int main()
{
	FILE* input, * output;
	errno_t err;
	char* inputname = (char*)malloc(FNSIZE * sizeof(char));
	char* outputname = (char*)malloc(FNSIZE * sizeof(char));
	char* line = (char*)malloc(N * sizeof(char));
	char symbol;
	char sep = 'x';
	char o;
	BigInt a, b, result;
	int base, to_base, op_counter;
	op_counter = 1;
	input = NULL;
	output = NULL;

	printf("Assuming that in.txt is in directory...\n");
	if ((err = fopen_s(&input, "in.txt", "r")) != 0)
	{
		printf("File in.txt not found. Please, enter file name manually. \n");
		printf("Input file name: ");
		scanf_s("%s", inputname, FNSIZE);
		printf("\n");
	}
	else {
		printf("in.txt FOUND!\n");
		fclose(input);
		strcpy_s(inputname, FNSIZE, "in.txt");
	}

	while (sep != 'y' && sep != 'n')
	{
		printf("Do you want your results to be stored in a separate file? (y/n) ");
		scanf_s(" %c", &sep, 1);
		if (sep == 'y')
		{
			printf("\nDo you want to use the default file name out.txt? (y/n) ");
			scanf_s(" %c", &o, 1);
			if (o == 'y') 
			{
				strcpy_s(outputname, FNSIZE, "out.txt");
			}
			else if (o == 'n')
			{
				printf("Output file name: ");
				scanf_s("%s", outputname, FNSIZE);
				printf("\n");
			}
			else
			{
				printf("Please provide a valid answer.\n");
				continue;
			}
		}
		else if (sep == 'n')
		{
			strcpy_s(outputname, FNSIZE, "tmpoutfile.txt");
			if (err = fopen_s(&output, outputname, "w") != 0)
			{
				printf("An error occured creating temporary output. Please check permissions \n");
				printf("Press ENTER to quit...\n");
				getchar(); getchar();
				return -1;
			}
			else fclose(output);
		}
		else
		{
			printf("Please provide a valid answer.\n");
		}
	}

	if ((inputname != NULL) && (outputname != NULL))
	{
		if ((err = fopen_s(&input, inputname, "r")) != 0)
		{
			printf("An error occured while reading %s. \n", inputname);
			printf("Press ENTER to quit...\n");
			getchar(); getchar();
			return -1;
		}
		if ((err = fopen_s(&output, outputname, "w")) != 0)
		{
			printf("An error occured while reading %s. \n", outputname);
			printf("Press ENTER to quit...\n");
			getchar(); getchar();
			return -1;
		}
	}
	else printf("Please enter valid filenames. \n");

	while (fgets(line, N, input) != NULL)
	{
		if (sscanf_s(line, "%d %d", &base, &to_base) == 2)
		{
			for (unsigned int i = 0; i < 4; i++)
			{
				if (fgets(line, N, input) != NULL)
				{
					if (i == 1) str_to_bigint(&a, line);
				}
			}
			printf("Calculating operation %d...   (%d -> %d) \n", op_counter, base, to_base);
			result = convert(a, base, to_base);

			// building output.txt
			fprintf(output, "%d %d\n", base, to_base);		// base_from base_to
			putc('\n', output);								//
			bigint_fprint(&a, output);						// a
			putc('\n', output);								// 
			bigint_fprint(&result, output);					// result
			putc('\n', output);								//

			printf("DONE!");
			printf("\n\n");
			op_counter++;
		}
		else if (sscanf_s(line, "%c %d", &symbol, 1, &base) == 2)
		{
			for (unsigned short i = 0; i < 6; i++)
			{
				if (fgets(line, N, input) != NULL)
				{
					if (i == 1)	str_to_bigint(&a, line);
					if (i == 3) str_to_bigint(&b, line);
				}
			}
			printf("Calculating operation %d...   (%c)\n", op_counter, symbol);

			// building output.txt
			fprintf(output, "%c %d\n", symbol, base);		// symbol base
			putc('\n', output);								//
			bigint_fprint(&a, output);						// a
			putc('\n', output);								// 
			bigint_fprint(&b, output);						// b
			putc('\n', output);								//

			//calculating.
			switch (symbol)
			{
			case '+':
				add(a, b, &result, base);
				break;
			case '*':
				multiply(a, b, &result, base);
				break;
			case '^':
				result = power(a, b, base);
				break;
			case '/':
				if (!is_zero(b)) divide(a, b, &result, base);
				else
				{
					printf("DIVISION_BY_ZERO!\n");
					fprintf(output, "DIVISION_BY_ZERO!\n");
					putc('\n', output);
					printf("\n");
					op_counter++;
					continue;
				}
				break;
			case '%':
				if (!is_zero(b)) result = remainder(a, b, base);
				else
				{
					printf("DIVISION_BY_ZERO!\n");
					fprintf(output, "DIVISION_BY_ZERO!\n");
					putc('\n', output);
					printf("\n");
					op_counter++;
					continue;
				}
				break;
			default:
				printf("Unknown operation. Check the input file. \n");
				fprintf(output, "Unknown operation. Check the input file. \n");
				putc('\n', output);
				printf("\n");
				op_counter++;
				continue;
			}

			bigint_fprint(&result, output);					// result
			putc('\n', output);								//

			printf("DONE!");
			printf("\n\n");
			op_counter++;

		}
	}


	// cleanup

	fclose(input);
	fclose(output);
	if (sep == 'n')
	{
		remove(inputname);
		rename(outputname, inputname);
	}
	bigint_free(&a);
	bigint_free(&b);
	bigint_free(&result);
	free(line);
	free(inputname);
	free(outputname);
	// wait for input
	printf("Press ENTER to quit...\n");
	getchar(); getchar();
	return 0;
}