/* data.c - data/read routines */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* scan the buffer for data statements
 * first scan, get a count of records,
 * then create the memory space to hold
 * these records. Scan from beginning again
 * but this time populate the newly created
 * memory space with the data in the statements.
 */

extern int *DataStorage;
extern int DataStorageMax;
extern int DataPosition;

int scanData(char *buffer, int lastpos) {

	int position=0;		// current position in the buffer;
	int n=0;
	char ln[6],keyword[12],line[80];
	int x;
	char *testline;
	char numstr[12];

bufscan:
	/* scan the buffer */
	memset(line,0,sizeof(line));
	n = 0;
	while (1) {
		line[n] = buffer[position+n];
		if (line[n] == '\n' || line[n] == '\0') break;
		n++;
		if (n+position >= lastpos) break;
	}
	if (n+position >= lastpos) goto step2;
	line[n] = '\0';
	position += (n+1);

	/* test the line for a data statement */
	//printf("bufscan: line [%s]\n",line);
	sscanf(line,"%s %s",ln,keyword);
	if (strcmp(keyword,"data")!=0) goto bufscan;

	/* keyword = data, count the elements in the line */
	/* format: 10 data 1,2,3,4,5\n\0  has 4 commas and 5 elements  */
	for (x=0; x<strlen(line); x++) 
		if (line[x] == ',') DataStorageMax++;
	DataStorageMax++;		// the final element
	goto bufscan;			// done with line

step2:
	//printf("bufscan: first scan complete\n");
	//printf("bufscan: number of data elements: %d\n",DataStorageMax);
	
	/* define storage space for the data elements */
	DataStorage = malloc((DataStorageMax+1)*sizeof(int));
	if (DataStorage == NULL) {
		printf("Memory Error allocating data elements\n");
		return -1;
	}

	/* now rescan the buffer, reading each data statement 
	 * and filling DataStorage with values 
	 */
	position=0;
	n=0;
bufload:
	memset(line,0,sizeof(line));
	n = 0;
	while (1) {
		line[n] = buffer[position+n];
		if (line[n] == '\n' || line[n] == '\0') break;
		n++;
		if (n+position >= lastpos) break;
	}
	if (n+position >= lastpos) goto step3;
	line[n] = '\0';
	position += (n+1);

	/* test the line for a data statement */
    //printf("bufscan: line [%s]\n",line);
    sscanf(line,"%s %s",ln,keyword);
    if (strcmp(keyword,"data")!=0) goto bufload;

	/* keyword = data, read and load elements */
	/* format: 10 data 1,2,3,4,5\n\0 */
	testline = line;
	if (isdigit(*testline)) while (isdigit(*testline)) testline++;	// skip line #
	if (isspace(*testline)) while (isspace(*testline)) testline++;	// and spaces
	if (isalpha(*testline)) while (isalpha(*testline)) testline++;	// and DATA statement
	if (isspace(*testline)) while (isspace(*testline)) testline++;	// final space

	/* the rest of the line holds a bunch of integers and commas */
bufload2:
	n = 0;
	memset(numstr,0,sizeof(numstr));
	while (1) {
		if (*testline == '\n' || *testline == '\0') break;
		if (*testline == ',') break;
		numstr[n] = *testline;
		n++; testline++;
	}
	
	/* value in intval */
	numstr[n+1] = '\0';
	//printf("data element: %s\n",numstr);
	DataStorage[DataPosition] = atoi(numstr);
	DataPosition++;
	testline += 1;		// skip the comma
	if (*testline == '\n' || *testline == '\0') goto bufload;	// done with this line
	goto bufload2;		// finish the line


step3:
	/* finished loading */
	DataStorageMax = DataPosition-1;		// max number of data elements
	/*
	printf("scanData: finished loading\n");
	printf("scanData: DataPosition %d\n",DataPosition);
	printf("Verification: \n");
	for (n=0; n<DataPosition; n++) printf("DataStorage[%d] = %d\n",n,DataStorage[n]);
	*/
	
	return 0;
}

	
int run_read(char *line) {		// load variables with data values
	extern int NumericVars[];
	extern int *DataStorage;
	extern int DataStorageMax;
	extern int DataPosition;

	if (isdigit(*line)) while (isdigit(*line)) line++;  // skip line #
    if (isspace(*line)) while (isspace(*line)) line++;  // and spaces
    if (isalpha(*line)) while (isalpha(*line)) line++;  // and READ statement
    if (isspace(*line)) while (isspace(*line)) line++;  // final space

	/* format: 1000 read a,b,c,d,e\n\0  */
	while (1) {
		if (*line == ',' || *line == ' ') {							// ignore spaces, commas
			line++;
			continue;
		}
		if (*line == '\n' || *line == '\0') return 0;				// normal end
		if (*line >= 'a' && *line <= 'z' && *(line+1) != '$') {		// just numeric vars
			if (DataPosition > DataStorageMax) {
				printf("Out Of Data\n");
				return -1;
			}
			NumericVars[*line-'a'] = DataStorage[DataPosition];
			DataPosition++;
			line++;
			continue;
		}
		printf("Unknown character in read statement\n");
		return -1;
	}

	return -1;
}


