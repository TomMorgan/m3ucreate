#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>



int doM = 0;				 // Is 1 if "-m" is selected, the command to write to the "m3u" to stdout instead of an m3u file.
int doF = 0;                             // Is 1 if "-f" is given as a command line argument, the command to write to an m3u file given as the last 		
					 // command line argument. 
int doA = 0;                             // Is 1 if "-a" is selected, the command to _APPEND_ to an existing m3u file.
int overrideM3UCheck = 0;                // Is 1 if "-o" is selected (write to last argument given even if it isn't an m3u file).
					 // Usage of "-o" can overwrite files so be careful!
int cmdI = 0;                            // Used to iterate through the command line arguments like "-f" or "-m" or "-o"
int noCMD = 0;
const char *version = "0.2";		                           // Used to specify whether a command line option has been entered.

int main (int argc, char *argv[]) {

	if (argc == 1) {

		printHelp();

		return 0;
	}		
	
	if (argc > 1) {

		int firstIsLastCommand = 0;
	
		for (cmdI=1; ((char) argv[cmdI][0] == '-'); cmdI++) {       // Iterate through the command lines, checking for '-'
		
			switch((char) argv[cmdI][1]) {
			
				case 'm': 
					doM=1; 
					break;
				case 'f':
					doF=1;
					break;
				case 'o':
					overrideM3UCheck=1;
					break;
				case 'a':
					doA=1;
					break;
				case '-':
					if (strcmp((char *) argv[cmdI] + 2, "help") == 0) {
						printHelp();
						firstIsLastCommand = 1;
						break;
					
					}

					else if	(strcmp((char *) argv[cmdI] + 2, "version") == 0) {
						printf("%s\n", version);
						firstIsLastCommand = 1;

						break;
					}

					break;

				default:
					firstIsLastCommand = 1;
					break;


					
			}

			if (firstIsLastCommand == 1) {
				break;
			}

		}

		if (cmdI == 1) {
			noCMD = 1;
		}

		
		
			

		if ((doM == 1) || (noCMD == 1) && (firstIsLastCommand == 0)) {        // If -m is given as a command or no command is specified... 
			
			for (int i = cmdI; i < argc; i++) {   // Iterate though every argument after the initial arguments.
			
				
				if (((doF == 1) && (i == argc - 1)) || (isM3U(argv[i]) == 1) || ((noCMD == 1) && (i == argc))) {
					break;
				}

				
				    
				int asteriskInFilename = 0;
					
				
				for (unsigned int j = 0; j < strlen(argv[i]); j++) {
						
					if (argv[i][(char) j] == '*') {                       // Check if there's an asterisk	  wildcard							
						asteriskInFilename = 1;
						glob_t globbuf;                         // Use glob to store all instances of wild
			 									// card in globbuf
								
					//	globbuf.gl_offs = 16384;                // Buffer overflow vulnerability
						if (glob(argv[i], 0, NULL, &globbuf) != 0) {
							printf("Problem with glob() or filesystem\n");
						}
								
						for (int k = globbuf.gl_pathc; k > 0; k--) { // Loop through globbuf.gl_pathv 											             // backwards to get alphabetized list
											     // and print it to m3ufile.	
							printf("%s\n", globbuf.gl_pathv[k]);
						}								
						break;
							
						
										
					}

		
					
				}	
				
				if (asteriskInFilename == 0) {			
					printf("%s\n", argv[i]); // Output arguments to screen
				}
				 	 // Print arguments to screen
			}

		}	
				

		if (doF == 1 || (isM3U(argv[argc - 1]) == 1) || doA == 1) {   // If "-f" has been given as a command or the last argument is an M3U (Write arguments to M3U file)

			if (argc > cmdI) {    // Make sure there actually are arguments to write.
	
	 			if (((isM3U(argv[argc - 1]) == 1) || (overrideM3UCheck == 1))) {        // Make sure last file given is actually
													   // an M3U to write, unless "-o" is true..

					FILE *m3ufile;
					if (doA == 0) { 
						                                                  
						m3ufile = fopen(argv[argc - 1], "w"); // Open file to write

					}
					else {
						m3ufile = fopen(argv[argc - 1], "a");
					}
										
					if (m3ufile == NULL) {
						fprintf(stderr, "Error opening M3U for writing\n");
						return -1;
					}

					

					for (int i = cmdI; i < argc - 1; i++) {     
						int asteriskInFilename = 0;
					
				
						for (char j = 0; j < strlen(argv[i]); j++) {
							
							if (argv[i][j] == '*') {                       // Check if there's an asterisk wildcard
								
								asteriskInFilename = 1;
								glob_t globbuf;                         // Use glob to store all instances of wild
													// card in globbuf
								
							//	globbuf.gl_offs = 16384;                // Buffer overflow vulnerability
								if (glob(argv[i], 0, NULL, &globbuf) != 0) {
									printf("Problem with glob() or filesystem\n");
								}
								
								for (int k = globbuf.gl_pathc; k > 0; k--) { // Loop through globbuf.gl_pathv 													             // backwards to get alphabetized list
													     // and print it to m3ufile.	
									fprintf(m3ufile, "%s\n", globbuf.gl_pathv[k]);
								}
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
					printf("No m3u file given\n");      // Tell if no M3U file has been given
				}
			}

			else {
				printf("No arguments for -f\n");  // Tell if no arguments for "-f" have been given
				
				return(1);

			}

			
		}

	
	}
		




	return (0);

	
}
int isM3U(char *filename1) {

	if (((strcmp((char *) filename1 + strlen(filename1) - 3, "m3u") == 0) || (strcmp((char *) filename1 + strlen(filename1) - 4, "m3u8") == 0))) 		{
		return 1;
	}
	else {
		return 0;
	}
}

void printHelp() {
	
	
		printf("\nm3ucreate version 0.2\n");
		printf("Creates M3U files from lists of files given as command line arguments. Asterisk wild\n");
		printf("cards are accepted for tracks.\n");
		printf("USAGE:\n");
		printf("-m plus files outputs to stdout\n");
		printf("-f plus music files followed by m3ufile outputs to m3ufile. Wildcards accepted.\n");
		printf("-o overrides requirement that m3ufile for \"-f\" ends in \"m3u\" or \"m3u8\"\n");
		printf("-a plus music files followed by existing m3ufile appends tracks to m3ufile.\n\n");

}
