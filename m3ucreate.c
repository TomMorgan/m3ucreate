#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

#define NO_USER_INTERFACE_OPTIMIZED 0
// ^ set as one to remove messages and features that wouldn't be seen if this was run without the commandline, stdout, or stderr being visible




char doM = 0;							// Is 1 if "-m" is selected, the command to write to the "m3u" to stdout instead of an m3u file.
char doF = 0;							// Is 1 if "-f" is given as a command line argument, the command to write to an m3u file given as the last 		
										// command line argument. 
char doA = 0;							// Is 1 if "-a" is selected, the command to _APPEND_ to an existing m3u file.
char doJ = 0;							// Is 1 if "-j" is selected, the command to join the contents of a list of m3us or tracks or any
										// combination of both
char overrideM3UCheck = 0;				// Is 1 if "-o" is selected (write to last argument given even if it isn't an m3u file).
										// Usage of "-o" can overwrite files so be careful!
int cmdI = 0;                           // Used to iterate through the command line arguments like "-f" or "-m" or "-o"
char noCMD = 0;
#if NO_USER_INTERFACE_OPTIMIZED == 0
const char *version = "0.2.3f";		                           // Used to specify whether a command line option has been entered.
#endif

char isM3U(char *filename1);            // Function prototypes; see functions themselves below main() for comments
#if NO_USER_INTERFACE_OPTIMIZED == 0
void printHelp();
#endif 

int main (int argc, char *argv[]) {

	

	if (argc == 1) {
		
		#if NO_USER_INTERFACE_OPTIMIZED == 0
		printHelp();
		#endif
		return 0;
	}		
	
	if (argc > 1) {

		int firstIsLastCommand = 0;
	
		for (cmdI=1; ((char) argv[cmdI][0] == '-'); cmdI++) {       // Iterate through the command lines, checking for '-'
		
			switch((char) argv[cmdI][1]) {
			
				case 'm': 
					doM=1;				// If -m is selected, doM = 1
										// (output to stdout)
					break;
				case 'f':
					doF=1;				// If -f is selected, doF = 1
										// (output to m3u file)
					break;
				case 'o':
					overrideM3UCheck=1; // If -o is selected, over-ride
										// check to see if last file ends in
										// "m3u" or "m3u8"
					break;
				case 'a':
					doA=1;              // If -a is selected, doA = 1
										// (append to m3u file)
					break;
				case 'j':
					doJ=1;				// If -j is selected, doJ = 1
										// (join any combination of filenames,
										// wildcards, or existing m3us into
										// new m3u)
					break;
				#if NO_USER_INTERFACE_OPTIMIZED == 0				
				case '-':
					if (strcmp((char *) argv[cmdI] + 2, "help") == 0) {	// If --help is selected...
						printHelp();									// ...print help to stdout
						firstIsLastCommand = 1;							// and don't check for more commands
						break;
					
					}

					else if	(strcmp((char *) argv[cmdI] + 2, "version") == 0) { // If --version is selected
						printf("%s\n", version);                                // print the version to stdout
						firstIsLastCommand = 1;									// and don't check for more commands

						break;
					}

					break;
				#endif

				default:
					firstIsLastCommand = 1; // If there are no valid commands, don't check for more commands
					break;
					
			}

			if (firstIsLastCommand == 1) {  // If there are no valid commands, break
				break;
			}

		}

		if (cmdI == 1) { // If command iterator (cmdI) is 1, there is no command.
			noCMD = 1;
		}

		
		
			

		if ((doM == 1) || (noCMD == 1) && (firstIsLastCommand == 0)) {        // If -m is given as a command or no command is specified... 
			
			for (int i = cmdI; i < argc; i++) {   // Iterate though every argument after the initial arguments.
			
				
				if (((doF == 1) && (i == argc - 1)) || (isM3U(argv[i]) == 1) || ((noCMD == 1) && (i == argc))) {
					break;
				}

				
				    
				char asteriskInFilename = 0;
					
				
				for (unsigned int j = 0; j < strlen(argv[i]); j++) {
						
					if (argv[i][(char) j] == '*') {				// Check if there's an asteriskwildcard
						asteriskInFilename = 1;
						glob_t globbuf;                         // Use glob to store all instances of wild
																// card in globbuf
								
						if (glob(argv[i], 0, NULL, &globbuf) != 0) {
							#if NO_USER_INTERFACE_OPTIMIZED == 0
							fprintf(stderr, "Problem with glob() or filesystem\n");	// If there's an error, say so..
							#endif
							return(-1);	// Exit with -1 (error)
						}
								
						for (int k = globbuf.gl_pathc; k > 0; k--) { 	// Loop through globbuf.gl_pathv 											             // backwards to get alphabetized list
																		// and print it to m3ufile.	
							printf("%s\n", globbuf.gl_pathv[k]);
						}
						
						globfree(&globbuf);
						break;
							
						
										
					}

		
					
				}	
				
				if (asteriskInFilename == 0) {			
					printf("%s\n", argv[i]); // Output arguments to screen
				}
				 
			}

		}	
				

		if (doF == 1 || (isM3U(argv[argc - 1]) == 1) || doA == 1) {   // If "-f" has been given as a command or the last argument is an M3U (Write arguments to M3U file)

			if (argc > cmdI) {    // Make sure there actually are arguments to write.
	
	 			if (((isM3U(argv[argc - 1]) == 1) || (overrideM3UCheck == 1))) {        // Make sure last file given is actually
																						// an M3U to write, unless "-o" is true..

					FILE *m3ufile;             // m3ufile is the m3u file to write.
					if (doA == 0) {								// Check if append mode is not on
						                                                  
						m3ufile = fopen(argv[argc - 1], "w");	// If it's not, open m3ufile as "write"

					}
					else {
						
						m3ufile = fopen(argv[argc - 1], "a");	// If it is, open existing m3u file as "append"
					}
										
					if (m3ufile == NULL) {
						#if NO_USER_INTERFACE_OPTIMIZED == 0
						fprintf(stderr, "Error opening M3U for writing\n");
						#endif
						return -1;
					}

					

					for (int i = cmdI; i < argc - 1; i++) {		// Iterate through commands 
						char asteriskInFilename = 0;			// Haven't found an asterisk yet, so it's 0
					
				
						for (int j = 0; j < strlen(argv[i]); j++) {
							
							if (argv[i][j] == '*') {                       // Check if there's an asterisk wildcard
								
								asteriskInFilename = 1;
								glob_t globbuf;							// Use glob to store all instances of wild
																		// card in globbuf
								
								if (glob(argv[i], 0, NULL, &globbuf) != 0) {
									#if NO_USER_INTERFACE_OPTIMIZED == 0
									fprintf(stderr, "Problem with glob() or filesystem\n");
									#endif
									return(-1);
								}
								
								for (int k = globbuf.gl_pathc; k > 0; k--) {		// Loop through globbuf.gl_pathv
																					// backwards to get alphabetized list
									fprintf(m3ufile, "%s\n", globbuf.gl_pathv[k]);	// and print it to m3ufile.	
								}
								
								globfree(&globbuf);
								break;
							
							}

						}


						if (asteriskInFilename == 0) {			
							fprintf(m3ufile, "%s\n", argv[i]);      // Output arguments to it
						}
					}

					fclose(m3ufile);                          // Close file

				}

				else {
					#if NO_USER_INTERFACE_OPTIMIZED == 0
					printf("No m3u file given\n");	// Tell if no M3U file has been given
					#endif
				}
			}

			else {
				#if NO_USER_INTERFACE_OPTIMIZED == 0
				printf("No arguments for -f\n");  // Tell if no arguments for "-f" have been given
				#endif				
				return(1);

			}

			
		}

		if (doJ == 1 || (isM3U(argv[argc - 1]) == 1)) { // If -j (doJ) has been selected, join existing m3u files, filenames,
														// and wildcards
			if (argc > cmdI) {                          // Check if there are actually more arguments than the command iterator
				
				if (((isM3U(argv[argc - 1]) == 1) || (overrideM3UCheck == 1))) {	// Check if the final argument is an m3u or override
																					// has been selected.
					FILE *m3uToWrite;										// m3uToWrite is the file pointer to the m3u that will be
																			// written to
					m3uToWrite = fopen(argv[argc - 1], "w");				// Open m3uToWrite in write mode

					if (m3uToWrite == NULL) {                               // Check for file errors
						#if NO_USER_INTERFACE_OPTIMIZED == 0
						fprintf(stderr, "File error with m3u to write");    // Say if there is one
						#endif
						return(-1);                                         // Exit if there is one
					}

					for (int i = cmdI; i < argc - 1; i++) {					// Iterate through arguments
						if (isM3U(argv[i]) == 1) {							// Check if argument is an m3u file
							//if (i == cmdI) {
								FILE *m3uToRead;							// m3uToRead is a file pointer to an m3u file to be read
								
								m3uToRead = fopen(argv[i], "r");			// Open m3u in read mode
								
								if (m3uToRead == NULL) {					// Check for file error
									#if NO_USER_INTERFACE_OPTIMIZED == 0
									fprintf(stderr, "M3UToRead cannot be read"); // Print that there's an error to stderr
									#endif
									return(-1);                                  // Exit program with -1 (error)
								}
 								int fileSize;                                    // fileSize is the size of the file
								
								fseek(m3uToRead, 0, SEEK_END);                   // Go to end of m3uToRead
								
								char *m3uStringBuf=malloc(ftell(m3uToRead) + 4);	// Allocate RAM for m3uStringBuf, which will hold
																					// the m3uToRead as a string
								
								if (m3uStringBuf == NULL) {							// Check for memory allocation error
									#if NO_USER_INTERFACE_OPTIMIZED == 0
									fprintf(stderr, "Error allocating memory, exiting.");	// Print that there's an error to stderr
									#endif
									return(-1);												// Exit -1
								}
								
								fseek(m3uToRead, 0, SEEK_SET);								// Seek to beginning of m3uToRead
								

								int j = 0;													// j is an iterator for reading into
																							// m3uStringBuf

								while(!(feof(m3uToRead))) {									// Contents of m3uToRead into m3uStringBuf
									m3uStringBuf[j] = fgetc(m3uToRead);
									j++;
								}
		
								m3uStringBuf[j - 1] = '\0';									// Append a 0x00 at end of m3uStringBuf
																							// (null terminated strings and all)
								fprintf(m3uToWrite, "%s", m3uStringBuf);					// Write m3uStringBuf to m3uToWrite
								fclose(m3uToRead);											// Close m3uToRead
								free(m3uStringBuf);											// Free RAM
								
						//	}
						}
						
						

						else {
							char asteriskInFilename = 0;						// By default, no asterisk until one is found
					
				
							for (char j = 0; j < strlen(argv[i]); j++) {   // Iterate through argument to look for asterisk
							
								if (argv[i][j] == '*') {                       // Check if there's an asterisk wildcard
								
									asteriskInFilename = 1;					// If there's an asterisk in filename, we have
																			// a wildcard
																			
									glob_t globbuf;                         // Use glob to store all instances of wild
																			// card in globbuf
								
									if (glob(argv[i], 0, NULL, &globbuf) != 0) {	// Set up glob, check if everything is OK
																					// (i.e., not 0)
										#if NO_USER_INTERFACE_OPTIMIZED == 0
										fprintf(stderr, "Problem with glob() or filesystem\n"); // Print error message
										#endif
										return(-1);  // Exit with -1 (error)
									}
									
									for (int k = globbuf.gl_pathc; k > 0; k--) {	// Loop through globbuf.gl_pathv 													             // backwards to get alphabetized list
																					// and print it to m3ufile.	
										fprintf(m3uToWrite, "%s\n", globbuf.gl_pathv[k]);
									}
									
									globfree(&globbuf);
									
									break;	// No need to check the rest of string
							
								}
										
							}

							if (asteriskInFilename == 0) {				// If there's no asterisk, then
																		// print argument to m3ufile.
								fprintf(m3uToWrite, "%s\n", argv[i]);   
								
							}
							
						}
						
						
						
					
					}
					
					if (m3uToWrite != NULL) {
							fclose(m3uToWrite);
					}
				}
				
			}
			
		}
		
	}
	
	return (0);
	
}

/* Checks if string ends in "m3u" or "m3u8". Used to keep from over-writing
 * files if someone forgets to supply an m3u as a last argument
 */

char isM3U(char *filename1) {

	if (((strcmp((char *) filename1 + strlen(filename1) - 3, "m3u") == 0) || (strcmp((char *) filename1 + strlen(filename1) - 4, "m3u8") == 0))) {
		return 1;
	}
	else {
		return 0;
	}
}

#if NO_USER_INTERFACE_OPTIMIZED == 0

/* Prints the help screen */

void printHelp() {
	
		printf("\nm3ucreate version 0.2.3f\n");
		printf("Creates M3U files from lists of files given as command line arguments. Asterisk wild\n");
		printf("cards are accepted for tracks.\n");
		printf("USAGE:\n");
		printf("-m plus files outputs to stdout\n");
		printf("-f plus music files followed by m3ufile outputs to m3ufile. Wildcards accepted.\n");
		printf("-o overrides requirement that m3ufile for \"-f\" ends in \"m3u\" or \"m3u8\"\n");
		printf("-a plus music files followed by existing m3ufile appends tracks to m3ufile.\n");
		printf("-j joins any combination of tracks, wildcards, and existing m3u files into an m3u file\n");
		printf("   supplied as the final argument.\n\n");

}

#endif
