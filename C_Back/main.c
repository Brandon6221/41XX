#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

int systemPassword;
typedef enum{ false=0, true=1 } bool;
//Basic data storage for each individual user
typedef struct userData{
	bool hopperAct [6]; //Hopper is in use = True, Hopper inactive = False
	int hopperSize [6]; //6 Size variants of pils for each hopper (1,2,3,4,5,6)
	int hopperNumDisp [6]; //Up to 3 dispense per hopper each cycle (1,2,3)
	int hopperTimes [6]; //Max 3 alarms per user, so each hopper can dispense at any combination of the 3 (1,2.3,4,5,6,7)
	/* 1 = Morning
	   2 = MidDay
	   3 = Evening
	   4 = Morning, MidDay
	   5 = Morning, Evening
	   6 = MidDay, Evening
	   7 = Morning, MidDay, Evening */
	int hopperDays [6]; //Combination of days of the week to dispense (X X X X X X X = M Tu W Th F Sa Su) and X = 0,1
	int userTone; //User selected alarm tone (0 . However many we choose)
	char userName [15]; //Name of the user
	int userAlarms [3]; //Set up to 3 per day. 0-1440 is on. 1440+ is disabled
	bool alarmsAct[3];
	bool passAct; //User may or may not have a password
	int userPass; //User password to update perscription (Can be Null)

}user;
//Digit length function
int lenHelper(int x) 
{
    if(x>=1000000000) return 10;
    if(x>=100000000) return 9;
    if(x>=10000000) return 8;
    if(x>=1000000) return 7;
    if(x>=100000) return 6;
    if(x>=10000) return 5;
    if(x>=1000) return 4;
    if(x>=100) return 3;
    if(x>=10) return 2;
    return 1;
}
//Runs users through the prompts required to initialize a user profile
//Will eventually be mostly barcode and data storage
user addUser(user soloUser)
{
	int menuSelect;
	int activate;
	FILE *fp;
	int i,j;
	char path[200];
	char temp[200];
	printf("Enter name for user\n"); 
	scanf("%s", soloUser.userName); //Enter user name using touch interface to scroll though letters and continue options
	if(strlen(soloUser.userName)<1)
	{
		strcpy(soloUser.userName,"delete"); //Null name marked for auto remove
	}
	i=0;
	j=0;
	while(i!=1)
	{
		system("raspistill -o image.jpg");
		i=0;
		
		//usleep(2000000);
		fp = popen("zbarimg image.jpg", "r");
		if (fp == NULL)
		{
			printf("Crash\n");
			strcpy(soloUser.userName,"delete");
			return(soloUser);
		}
		//usleep(4000000);
		if(fgets(path, 200, fp)==NULL)
		{
			strcpy(path,"null");
		}
		else{
			path[strlen(path)-1]='\0';		
		}
		//printf("!!!%s!!!\n", path);
		if(j==4)
		{
			printf("Invalid barcode. Too many invalid scans. Returning to previous menu\n");
			break;
		}
		else if (path[0]=='Q')
		{	
			printf("Successful barcode scan\n");
			i=1;
		}
		else if(path[0]=='n')
		{
			printf("Invalid barcode. Rescanning");
			i=0;
			j++;
		}
	pclose(fp);
	//printf("!!!%s!!!\n", path);
	//usleep(2000000);
	}
	//system("zbarimg image.jpg");
	if(i==1)
	{
		printf("%s\n",path);
		for(j=0; j<6; j++)
		{
			if(path[8+j]=='1')
			{
				soloUser.hopperAct[j]=true;
			}
			else
			{
				soloUser.hopperAct[j]=false;
			}
			soloUser.hopperSize[j] = path[14+j] - '0';	
			soloUser.hopperNumDisp[j] = path[20+j] - '0';
			soloUser.hopperTimes[j] = path[26+j] - '0';
		}
		printf("Hi\n");
		for(j=0;j<7;j++)
		{
			strncpy(temp, &path[32+(7*j)],7);
			soloUser.hopperDays[j]=atoi(temp);
		}
		printf("Hi\n");
	}
	else{
		strcpy(soloUser.userName,"delete");
		return(soloUser);
	}	
	
	//printf("!!!%s!!!", temp);

	//This loop will be a prompt to scan a barcode. Process data and store. Still needs error checking for in range numbers
	/*
	Start barcode scan process after user name confirmed
	Barcode actively watches unit a barcode is returned or timeout occurs on scan (30 seconds)
	On timeout return to System Menu
	If proper barcode, check date verification code
	if proper, accept barcode and place data in the users struct
	If improper print barcode error and return to system menu
	*/
	/*while(menuSelect!=2)
	{
		printf("Enter Hopper number to activate (1-6)\n"); //For data entry purpose only. Will be provided by Barcode
		scanf("%d", &activate);
		if(activate<7 && activate>0)
		{	
			soloUser.hopperAct[activate-1]=true;
			printf("Pill size in Hopper %d (1-6):\n", activate); //For data entry purpose only. Will be provided by Barcode
			scanf("%d", &soloUser.hopperSize[activate-1]);
			printf("Pills from Hopper %d each serving (1-3):\n", activate); //For data entry purpose only. Will be provided by Barcode
			scanf("%d", &soloUser.hopperNumDisp[activate-1]);
			printf("Alarms at which to recieve pills from Hopper %d (1-7):\n", activate); //For data entry purpose only. Will be provided by Barcode
			scanf("%d", &soloUser.hopperTimes[activate-1]);
			printf("Days of the week to dispense from Hopper %d (XXXXXXX) [0 or 1]:\n", activate); //For data entry purpose only. Will be provided by Barcode
			scanf("%d", &soloUser.hopperDays[activate-1]);
			printf("Would you like to add another Hopper to user: %s? 1 = Yes, 2 = No\n", soloUser.userName); //For data entry purpose only. Will be provided by Barcode
			scanf("%d", &menuSelect);
		}
		else{
			printf("Invalid Hopper\n"); //For data entry purpose only. Will be provided by Barcode
		}
	}*/
	printf("Would you like to change preset alarm times?(8AM, 2PM, 8PM) 1 = Yes, 2 = No\n"); //Change preset Alarms on account creation
	scanf("%d", &activate);

	//Impliment using touch interface to scroll between the 3 Alarms. Then use a sub menu to scroll between Hr and Min
	//Boolean logic ensures alarms stay in respective timing zones and also ensures minimum 2 hour buffer between alarms
	if(activate == 1)
	{
		while(activate == 1)
		{	
			//Will be a display window with 3 options to scroll between
			printf("Current Alarms: %d:%d, %d:%d, %d:%d\n",soloUser.userAlarms[0]/60,soloUser.userAlarms[0]%60,soloUser.userAlarms[1]/60,soloUser.userAlarms[1]%60,soloUser.userAlarms[2]/60,soloUser.userAlarms[2]%60);
			printf("Which alarm to change? 1 = Morning, 2 = MidDay, 3 = Evening\n");
			scanf("%d", &activate);
			switch(activate){
				case 1:
					//Logic is good, needs to scroll between changing hour and minutes. Possibly lock scroll at max and min point?
					printf("Set Morning alarm to when? (Minutes and military) (After 4:00AM . 12PM)\n");
					scanf("%d", &i);
					if(i>1440 || i<480 || i>720 || i>soloUser.userAlarms[1]-120 || (i<soloUser.userAlarms[2]+120 && soloUser.userAlarms[2]>=0 && soloUser.userAlarms[2]<480))
					{
						printf("Invalid Morning alarm (Before 2AM or after 12PM)\n");
					}
					else{
						soloUser.userAlarms[0]=i;
					}
					break;
				case 2:
					//Logic is good, needs to scroll between changing hour and minutes. Possibly lock scroll at max and min point?
					printf("Set MidDay alarm to when? (Minutes and military) (After 12:00AM . 6PM)\n");
					scanf("%d", &i);
					if(i>1440|| i<720 || i>1080 || (i>soloUser.userAlarms[2]-120 && soloUser.userAlarms[2]>480) || soloUser.userAlarms[0]+120>i)
					{
						printf("Invalid MidDay alarm (Before 12PM or after 6PM)\n");
					}
					else{
						soloUser.userAlarms[1]=i;
					}
					break;
				case 3:
					//Logic is good, needs to scroll between changing hour and minutes. Possibly lock scroll at max and min point?
					printf("Set Evening alarm to when? (Minutes and military) (After 6PM . 4AM)\n");
					scanf("%d", &i);
					if(i>1440 || (i<1080 && i>480) || i<0 || (i>soloUser.userAlarms[0]-120 && i<480) || (i<soloUser.userAlarms[1]+120 && i>1080))
					{
						printf("Invalid Evening alarm (Before 6PM or after 4AM)\n");
					}
					else{
						soloUser.userAlarms[2]=i;
					}
					break;
				default:
					printf("Invalid alarm selection\n");
					break;
			}
			printf("Press 1 to change alarms further or 2 to continue\n");
			scanf("%d", &activate);
		}
	}
	//Apply a 1-5 digit integer code to account
	//Impliment using touch scroll interface but with integers and a confirm option
	printf("Would you like to enable a password? 1 = Yes, 2 = No\n");
	scanf("%d",&j);
	if(j==1)
	{	
		soloUser.userPass=0;
		while(lenHelper(soloUser.userPass)<3 || lenHelper(soloUser.userPass)>5)
		{
			printf("What pin would you like to set? (Up to 5 digits)\n");
			scanf("%d",&soloUser.userPass);
			if(lenHelper(soloUser.userPass)<3)
			{
				printf("Passwords of shorter than 3 digit length are not accepted.\n");
			}
			else if(lenHelper(systemPassword)>5)
			{
				printf("Passwords greters than 5 digit length are not accepted.\n");
			}
		}
	}
	printf("New User: %s Added Successfully!\n", soloUser.userName);

	return(soloUser);
}
//Init a User to Null
user clearUser(user soloUser)
{
	//Initializes a user profile to NULL
	int i;
	strcpy(soloUser.userName,"\0");
	for(i=0;i<6; i++)
	{
		soloUser.hopperAct[i] = false;
		soloUser.hopperSize[i] = 0;
		soloUser.hopperNumDisp[i] = 0;
		soloUser.hopperTimes[i] = 0;
		soloUser.hopperDays[i] = 0;
	}
	soloUser.userTone = 0;
	soloUser.userAlarms[0] = 480;
	soloUser.userAlarms[1] = 840;
	soloUser.userAlarms[2] = 1200;
	bool passAct = false;
	return(soloUser);
}
//Allows acces to Perscription changes, alarm tone and time changes, and password setting
user userMenu(user soloUser)
{
	int menuSelect, i,j,k;
	//User Menu process loop
	while(1)
	{	
		k=0;
		j=1;
		printf("Hello %s\nWhat would you like to do?\n1: Change Alarm Time\n2: Change Alarm Tone\n3: Update Perscription\n4: User Password Settings\n5: Return to Main Menu\n", soloUser.userName);
		scanf("%d", &menuSelect);
		switch(menuSelect){
			//Leads to display allowing selection of which time to update
			case 1:
				while(j == 1)
				{	
					//Will be a display window with 3 options to scroll between
					printf("Current Alarms: %d:%d, %d:%d, %d:%d\n",soloUser.userAlarms[0]/60,soloUser.userAlarms[0]%60,soloUser.userAlarms[1]/60,soloUser.userAlarms[1]%60,soloUser.userAlarms[2]/60,soloUser.userAlarms[2]%60);
					printf("Which alarm to change? 1 = Morning, 2 = MidDay, 3 = Evening\n");
					scanf("%d", &j);
					switch(j){
						case 1:
							//Logic is good, needs to scroll between changing hour and minutes. Possibly lock scroll at max and min point?
							printf("Set Morning alarm to when? (Minutes and military) (After 4:00AM . 12PM)\n");
							scanf("%d", &i);
							if(i>1440 || i<480 || i>720 || i>soloUser.userAlarms[1]-120 || (i<soloUser.userAlarms[2]+120 && soloUser.userAlarms[2]>=0 && soloUser.userAlarms[2]<480))
							{
								printf("Invalid Morning alarm (Before 2AM or after 12PM)\n");
							}
							else{
								soloUser.userAlarms[0]=i;
							}
							break;
						case 2:
							//Logic is good, needs to scroll between changing hour and minutes. Possibly lock scroll at max and min point?
							printf("Set MidDay alarm to when? (Minutes and military) (After 12:00AM . 6PM)\n");
							scanf("%d", &i);
							if(i>1440|| i<720 || i>1080 || (i>soloUser.userAlarms[2]-120 && soloUser.userAlarms[2]>480) || soloUser.userAlarms[0]+120>i)
							{
								printf("Invalid MidDay alarm (Before 12PM or after 6PM)\n");
							}
							else{
								soloUser.userAlarms[1]=i;
							}
							break;
						case 3:
							//Logic is good, needs to scroll between changing hour and minutes. Possibly lock scroll at max and min point?
							printf("Set Evening alarm to when? (Minutes and military) (After 6PM . 4AM)\n");
							scanf("%d", &i);
							if(i>1440 || (i<1080 && i>480) || i<0 || (i>soloUser.userAlarms[0]-120 && i<480) || (i<soloUser.userAlarms[1]+120 && i>1080))
							{
								printf("Invalid Evening alarm (Before 6PM or after 4AM)\n");
							}
							else{
								soloUser.userAlarms[2]=i;
							}
							break;
						default:
							printf("Invalid alarm selection\n");
							break;
					}
					printf("Press 1 to change alarms further or 2 to continue\n");
					scanf("%d", &j);
				}
				break;
			//Allows updating of tone number. Actual tone selection done during alarm sequence by reading preference
			case 2:
				printf("What alarm tone would you like to use?(1-16)\n");
				scanf("%d",&j);
				if(j>16 || j<0)
				{
					printf("Invalid alarm tone\n");
				}
				else{
					soloUser.userTone = j;
				}
				break;
			//Allows addition of new hopper to perscription. Not realistic, as it will only be a barcode scan
			case 3:
				while(j!=2)
				{
					//Will likely call the barcode function used after name input at new user menu
					printf("Enter Hopper number to activate (1-6)\n"); //For data entry purpose only. Will be provided by Barcode
					scanf("%d", &j);
					if(j<7 && j>0)
					{	
						soloUser.hopperAct[j-1]=true;
						printf("Pill size in Hopper %d (1-6):\n", j); //For data entry purpose only. Will be provided by Barcode
						scanf("%d", &soloUser.hopperSize[j-1]);
						printf("Pills from Hopper %d each serving (1-3):\n", j); //For data entry purpose only. Will be provided by Barcode
						scanf("%d", &soloUser.hopperNumDisp[j-1]);
						printf("Alarms at which to recieve pills from Hopper %d (1-7):\n", j); //For data entry purpose only. Will be provided by Barcode
						scanf("%d", &soloUser.hopperTimes[j-1]);
						printf("Days of the week to dispense from Hopper %d (XXXXXXX) [0 or 1]:\n", j); //For data entry purpose only. Will be provided by Barcode
						scanf("%d", &soloUser.hopperDays[j-1]);
						printf("Would you like to add another Hopper to user: %s? 1 = Yes, 2 = No\n", soloUser.userName); //For data entry purpose only. Will be provided by Barcode
						scanf("%d", &j);
					}
					else{
						printf("Invalid Hopper\n"); //For data entry purpose only. Will be provided by Barcode
					}
				}
				break;
			//Password altercation menu. Digit guard not in place yet.
			case 4:
				if(soloUser.passAct==false)
				{
					printf("Would you like to enable a password? 1 = Yes, 2 = No\n");
					scanf("%d",&j);
					if(j==1)
					{
						//New password, so can assum will enter loop
						while(lenHelper(soloUser.userPass)<3 || lenHelper(soloUser.userPass)>5)
						{
							printf("What pin would you like to set? (Up to 5 digits)\n");
							scanf("%d",&soloUser.userPass);
							if(lenHelper(soloUser.userPass)<3)
							{
								printf("Passwords of shorter than 3 digit length are not accepted.\n");
							}
							else if(lenHelper(systemPassword)>5)
							{
								printf("Passwords greters than 5 digit length are not accepted.\n");
							}
						}
					}
				}
				else{
					printf("Would you like to: 1 = Remove your password, 2 = Change your password, 3 = Return?\n");
					scanf("%d",&j);
					if(j==1 || j==2)
					{
						//REquires current password before changing
						printf("Enter your current password:\n");
						scanf("%d",&k);
						if(k==soloUser.userPass)
						{
							if(j==1)
							{
								soloUser.passAct=false;
								soloUser.userPass=0;
								printf("Password removed\n");
							}
							else{
								//Slightly different than other password updates as it saves original incase of exiting	
								k=0;
								while(lenHelper(k)<3 || lenHelper(k)>5)
								{
									printf("What pin would you like to set? (Up to 5 digits)\n");
									scanf("%d",&k);
									if(lenHelper(k)<3)
									{
										printf("Passwords of shorter than 3 digit length are not accepted.\n");
									}
									else if(lenHelper(k)>5)
									{
										printf("Passwords greters than 5 digit length are not accepted.\n");
									}
									else{
										soloUser.userPass=k;
									}
								}
							}
						}
						else{
							printf("Wrong password, returning to menu\n");
						}
					}
				}
				break;
			case 5:
				return(soloUser);
				break;
			default:
				printf("Invalid Menu Selection\n");
				break;	
		}
	}
}
//Writes all current configurations to save text file in storage arrangement seen in README
//Terminates with the system known string
void save(user* Users, int numUsers)
{
	int i,j;
	FILE * filePtr;
	filePtr = fopen("save.txt","w");
	fprintf(filePtr, "%d\n", systemPassword);
	for(i=0;i<numUsers;i++)
	{
		fprintf(filePtr,"%s\n", Users[i].userName);
		for(j=0;j<6;j++)
		{
			if(Users[i].hopperAct[j]==true)
			{
				fprintf(filePtr,"1 ");
			}
			else{
				fprintf(filePtr,"0 ");
			}
		}
		fprintf(filePtr,"\n");
		for(j=0;j<6;j++)
		{
			fprintf(filePtr,"%d ", Users[i].hopperSize[j]);
		}
		fprintf(filePtr,"\n");
		for(j=0;j<6;j++)
		{
			fprintf(filePtr,"%d ", Users[i].hopperNumDisp[j]);
		}
		fprintf(filePtr,"\n");
		for(j=0;j<6;j++)
		{
			fprintf(filePtr,"%d ", Users[i].hopperTimes[j]);
		}
		fprintf(filePtr,"\n");
		for(j=0;j<6;j++)
		{
			fprintf(filePtr,"%d ", Users[i].hopperDays[j]);
		}
		fprintf(filePtr,"\n");
		fprintf(filePtr,"%d\n", Users[i].userTone);
		for(j=0;j<3;j++)
		{
			fprintf(filePtr,"%d ", Users[i].userAlarms[j]);
		}
		fprintf(filePtr,"\n");
		if(Users[i].passAct==true)
		{
			fprintf(filePtr,"1 ");
		}
		else{
			fprintf(filePtr,"0 ");
		}
		fprintf(filePtr,"\n");
		fprintf(filePtr,"%d ", Users[i].userPass);
		fprintf(filePtr,"\n");
		
	}
	fprintf(filePtr,"1234567890\n");
	fclose(filePtr);
}
//Reads from the save file to load prefernces from a shutdown. Requires a password on first line of save file and the terminator string on the second to function
//Terminating string is 1234567890
int load(user *Users)
{
	char temp[50];
	int i=0;
	int j;
	FILE * filePtr;
	filePtr = fopen("save.txt","r");
	fgets(temp, 50, filePtr);
	sscanf(temp, "%d", &systemPassword);
	while(strcmp(fgets(temp, 50, filePtr),"1234567890\n")!=0)
	{
		temp[strlen(temp)-1] = '\0';
		strcpy(Users[i].userName,temp);
		fgets(temp, 50, filePtr);
		for(j=0; j<6; j++)
		{	
			if(temp[2*j]=='1')
			{
				Users[i].hopperAct[j] = true;
			}
			else{
				Users[i].hopperAct[j] = false;
			}
		}
		fgets(temp, 50, filePtr);
		sscanf(temp, "%d %d %d %d %d %d", &Users[i].hopperSize[0], &Users[i].hopperSize[1], &Users[i].hopperSize[2], &Users[i].hopperSize[3], &Users[i].hopperSize[4], &Users[i].hopperSize[5]);
		fgets(temp, 50, filePtr);
		sscanf(temp, "%d %d %d %d %d %d", &Users[i].hopperNumDisp[0], &Users[i].hopperNumDisp[1], &Users[i].hopperNumDisp[2], &Users[i].hopperNumDisp[3], &Users[i].hopperNumDisp[4], &Users[i].hopperNumDisp[5]);
		fgets(temp, 50, filePtr);
		sscanf(temp, "%d %d %d %d %d %d", &Users[i].hopperTimes[0], &Users[i].hopperTimes[1], &Users[i].hopperTimes[2], &Users[i].hopperTimes[3], &Users[i].hopperTimes[4], &Users[i].hopperTimes[5]);
		fgets(temp, 50, filePtr);
		sscanf(temp, "%d %d %d %d %d %d", &Users[i].hopperDays[0], &Users[i].hopperDays[1], &Users[i].hopperDays[2], &Users[i].hopperDays[3], &Users[i].hopperDays[4], &Users[i].hopperDays[5]);
		fgets(temp, 50, filePtr);
		sscanf(temp, "%d", &Users[i].userTone);
		fgets(temp, 50, filePtr);
		sscanf(temp, "%d %d %d", &Users[i].userAlarms[0], &Users[i].userAlarms[1], &Users[i].userAlarms[2]);
		fgets(temp, 50, filePtr);
		sscanf(temp, "%d", &j);
		if(j==1)
		{
			Users[i].passAct = true;
		}
		else{
			Users[i].passAct = false;
		}
		fgets(temp, 50, filePtr);
		sscanf(temp, "%d", &Users[i].userPass);
		i++;
	}
	return(i);
}
//The system setting menu of the GUI
int systemSettings(user* Users, int numUsers)
{
	int i,k;
	int menuSelect = 0;
	FILE * filePtr;
	printf("%d\n", numUsers);
	printf("Please input system password to conitune\n");
	scanf("%d",&i);
	//Return to main menu on bad password
	if(i!=systemPassword)
	{
		printf("Invalid password, returning to Main Menu\n");
		return(numUsers);
	}
	else{
		//Emulates the basic text based menu for the system setting
		while(menuSelect!=5)
		{
			printf("Login Complete\nWhat would you like to do?\n1: Add a User\n2: Remove a User\n3: Erase Settings\n4: Change system password\n5: Return to Main Menu\n");
			scanf("%d", &menuSelect);
			switch(menuSelect){
				case 1:
					//Max user check for adding users. 5 Max
					if(numUsers==5)
					{
						printf("Max users reached\n");
					}
					else{
						Users[numUsers] = clearUser(Users[numUsers]);
						Users[numUsers] = addUser(Users[numUsers]);
						numUsers++;
						save(Users, numUsers);
					}
					break;
				case 2:
					//Delete a user. has a check for no users left
					printf("Which user would you like to delete?\n");
					scanf("%d",&i);

					if(i>numUsers || numUsers == 0)
					{
						printf("Invalid User or no users to delete\n");
					}
					else{
						strcpy(Users[i-1].userName,"delete");
						save(Users, numUsers);
					}
					break;
				case 3:
					//Prompts to ensure before deleting save file and restoring with basic neede contents
					printf("Are you sure you would like to clear all user customizations? 1 = Yes, 2 = No\n");
					scanf("%d",&i);
					if(i==1)
					{
						filePtr = fopen("save.txt","w");
						fprintf(filePtr,"0\n1234567890\n");
						fclose(filePtr);
						numUsers = load(Users);
					}
					break;
				//System password change
				case 4:
					printf("Please input current system password\n");
					scanf("%d",&i);
					if(i==systemPassword)
					{
						k=0;
						while(lenHelper(k)<3 || lenHelper(k)>5)
						{
							printf("What pin would you like to set? (Up to 5 digits)\n");
							scanf("%d",&k);
							if(lenHelper(k)<3)
							{
								printf("Passwords of shorter than 3 digit length are not accepted.\n");
							}
							else if(lenHelper(k)>5)
							{
								printf("Passwords greters than 5 digit length are not accepted.\n");
							}
							else{
								systemPassword=k;
							}
						}
					}
					else{
						printf("Invalid password, returning to menu\n");
					}
					printf("%d\n", numUsers);
					break;
				case 5:
					//Return to main menu and save
					return(numUsers);
					break;
				default:
					printf("Invalid option\n");
					menuSelect=0;
					break;
			}
		}

	}
}

void watcherPID(int PID, int i, bool *userChild, int *userProcessID, user *Users, int numUsers)
{
	char temp[100];
	time_t rawtime;
  	struct tm * timeinfo;
  	char day[10];
  	char month[10];
  	int date;
  	int hr,min,sec,yr;
  	int j,k;
	if(PID==0)
	{
		//i--;
		//Mark all userChild processes as false except one this PID is servicing
		for(j=i-1;j>-1;j--)
		{
			userChild[j] = false;
		}
		for(j=i+1;j<5;j++)
		{
			userChild[j] = false;
		}
		//Locate process index and begin sleep/watch cycle
		for(i=0;i<5;i++)
		{
			if(userChild[i]==true)
			{
				//Process Loop
				while(1)
				{
					usleep(10000000);	//Awake every 10 seconds to check for Medication time
			  		time(&rawtime);
					timeinfo = localtime(&rawtime);
					strcpy(temp,asctime(timeinfo));
					sscanf(temp, "%s %s %d %d:%d:%d %d", day, month, &date, &hr, &min, &sec, &yr);
					//Compares current time against alarm times
					for(j=0;j<3;j++)
					{
						//On match print warning to screen and sleep for 60 seconds
						if((hr*60)+min == Users[i].userAlarms[j])
						{
							//printf("%s\n", day);
							printf("TAKE YOUR MEDICATION %s!!! Current Time is ",Users[i].userName);
							if(hr>12)
							{
								printf("%d:%d\n", hr-12, min);
							}
							else{
								printf("%d:%d\n",hr ,min);
							}
							numUsers = load(Users);
							//printf("Hi\n");
							for(k=0; k<6; k++)
							{
								//Guarentees active hopper (Broken)
								if(Users[i].hopperAct[k]==true)
								{
									//Ensures pill is subscribed from the alarm sounding ()Broken
									if((j==0 && (Users[i].hopperTimes[k]==1 || Users[i].hopperTimes[k]==4 || Users[i].hopperTimes[k]==5 || Users[i].hopperTimes[k]==7)) || (j==1 && (Users[i].hopperTimes[k]==2 || Users[i].hopperTimes[k]==4 || Users[i].hopperTimes[k]==6 || Users[i].hopperTimes[k]==7)) || (j==2 && (Users[i].hopperTimes[k]==3 || Users[i].hopperTimes[k]==5 || Users[i].hopperTimes[k]==6 || Users[i].hopperTimes[k]==7)))
									{
										if((Users[i].hopperDays[k]/10/10/10/10/10/10%10==1 && strcmp(day, "Mon")==0) || (Users[i].hopperDays[k]/10/10/10/10/10%10==1 && strcmp(day, "Tue")==0) || (Users[i].hopperDays[k]/10/10/10/10%10==1 && strcmp(day, "Wed")==0) || (Users[i].hopperDays[k]/10/10/10%10==1 && strcmp(day, "Thu")==0) || (Users[i].hopperDays[k]/10/10%10==1 && strcmp(day, "Fri")==0) || (Users[i].hopperDays[k]/10%10==1 && strcmp(day, "Sat")==0) || (Users[i].hopperDays[k]%10==1 && strcmp(day, "Sun")==0))
										{
											printf("Hopper %d distributes %d pills\n",k+1, Users[i].hopperNumDisp[k]);
											//Distribute (hopperNumDisp[k]) pills from hopper (k), of size variation (hopperSize[k]). These pills distribute at alarms set in (hopperTimes[k]) of the day, on days (hopperDays[k]) of the week
										}
									}
								}	
							}
							usleep(60000000);
						}
					}
				}
			}
		}
	}
	return;
}

int main()
{
	int i,j,k;
	int menuSelect;
	int numUsers = 0;
	char temp[100];
	time_t rawtime;
  	struct tm * timeinfo;
  	char day[10];
  	char month[10];
  	int date;
  	int hr,min,sec,yr;
  	int PID;
  	bool userChild[5] = {false, false, false, false};
	int userProcessID[5] = {0, 0, 0, 0, 0};
	systemPassword=0;

	user * Users = malloc(sizeof(struct userData)*6);
	for(i=0; i<5; i++);
	{
		Users[i]=clearUser(Users[i]);
		userChild[i]=false;
	}

	//Calls load function to read users config from save text
	numUsers = load(Users);

	//Ensures a password is set for global system
	while(lenHelper(systemPassword)<3 || lenHelper(systemPassword)>5)
	{
		printf("Please choose a global system Password\n");
		scanf("%d", &systemPassword);
		if(lenHelper(systemPassword)<3)
		{
			printf("Passwords of shorter than 3 digit length are not accepted.\n");
		}
		else if(lenHelper(systemPassword)>5)
		{
			printf("Passwords greters than 5 digit length are not accepted.\n");
		}
	}
	//Functioning loop of the process
	while(1)
	{
		//Killing of all old, possibly outdated processes to make way for new ones
  		for(i=0;i<5;i++)
  		{
  			if(userChild[i] == true)
  			{
  				printf("Killing %d\n", i+1);
  				kill(userProcessID[i], SIGTERM);
  			}
  		}
  		//Creates processes equal to that of users
  		for(i=0;i<numUsers;i++)
  		{
  			//Only allows creation if parents process. This way it generate a process for each user and each break the for loop
  			if(PID!=0)
  			{
  				userChild[i] = true;
  				PID = fork();
  				userProcessID[i]=PID;
  			}
  			else{
  				break;	
  			}
  		}
  		//Child Process 
  		if(PID==0)
  		{
  			watcherPID(PID, i-1, userChild, userProcessID, Users, numUsers);
  		}
  		//Wipe PID init for deleted processes
  		for(i=numUsers; i<5;i++)
  		{
  			userChild[i] = false;
  		}

		//First lists current users
		printf("\nUsers:\n\n");
		if(numUsers==0)
		{
			printf("No users yet\n\n");
		}
		for(i=0; i<numUsers; i++)
		{
			printf("\nUser %d:\nName: %s\n\n", i+1, Users[i].userName);
		}
		//Presents Main menu options
		printf("\nWhat would you like to do?\n1: Select User Menu (followed by user #)\n2: System Settings\n3: Save & Exit\n\n");

		scanf("%d", &menuSelect);

		switch(menuSelect){
			case 1:
				//userMenu unfinnished
				scanf("%d", &menuSelect);
				printf("Selecting User\n");
				Users[menuSelect-1] = userMenu(Users[menuSelect-1]);
				save(Users, numUsers);
				break; 
			case 2:
				//System setting completed. Needs thorough testing
				printf("Accessing Settings\n");
				numUsers = systemSettings(Users, numUsers);
				save(Users, numUsers);
				break; 
			case 3:
				//Saves and closes. Most eligent and simple menu function 
				printf("Exiting\n");
				save(Users, numUsers);
				//Cleans up process' on exit
				for(i=0;i<5;i++)
		  		{
		  			if(userChild[i] == true)
		  			{
		  				printf("Killing %d\n", i+1);
		  				kill(userProcessID[i], SIGTERM);
		  			}
		  		}
				exit(0);
				break; 
			default:
				printf("Invalid Input\n");
				break; 
		}
		//Clean up Users Array incase of deletion previous loop (ie User1, User2, "deleted user", User4 -> User1, User2, User3)
		for(i=0;i<numUsers;i++)
		{
			if(strcmp(Users[i].userName,"delete")==0)
			{
				for(j=i;j<numUsers;j++)
				{
					Users[j]=Users[j+1];
				}
				numUsers--;	
			}

		}
	}
	return(0);
}
