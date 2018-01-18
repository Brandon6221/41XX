#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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
//Runs users through the prompts required to initialize a user profile
//Will eventually be mostly barcode and data storage
user addUser(user soloUser)
{
	int menuSelect;
	int activate;
	int i;
	printf("Enter name for user\n"); 
	scanf("%s", soloUser.userName); //Enter user name using touch interface to scroll though letters and continue options
	if(strlen(soloUser.userName)<1)
	{
		strcpy(soloUser.userName,"delete"); //Null name marked for auto remove
	}
	//This loop will be a prompt to scan a barcode. Process data and store
	while(menuSelect!=2)
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
	}
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
	printf("Would you like to password protect you account? 1 = Yes, 2 = No\n");
	scanf("%d", &activate);
	if(activate==1)
	{
		printf("Please insert desired Password (5 digit max)\n");
		scanf("%d", &soloUser.userPass);
		soloUser.passAct=true;
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
//Unfinnished
user userMenu(user soloUser)
{
	int menuSelect;
	printf("What would you like to do?\n1: Change Alarm Time\n2: Change Alarm Tone\n3: Update Perscription\n4: User Password Settings\n");
	scanf("%d", &menuSelect);
	strcpy(soloUser.userName,"delete");
	return (soloUser);
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
	int i;
	int menuSelect = 0;
	FILE * filePtr;
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
		while(menuSelect!=4)
		{
			printf("Login Complete\nWhat would you like to do?\n1: Add a User\n2: Remove a User\n3: Erase Settings\n4: Return to Main Menu\n");
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

					if(i>=numUsers || numUsers == 0)
					{
						printf("Invalid User or no users to delete\n");
					}
					else{
						strcpy(Users[i].userName,"delete");
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
				case 4:
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

int main()
{
	int i,j,k;
	int menuSelect;
	int numUsers = 0;
	char temp[50];
	systemPassword=0;

	user * Users = malloc(sizeof(struct userData)*6);
	for(i=0; i<5; i++);
	{
		Users[i]=clearUser(Users[i]);
	}

	//Calls load function to read users config from save text
	numUsers = load(Users);

	//Ensures a password is set for global system
	if(systemPassword==0)
	{
		printf("Please choose a global system Password\n");
		scanf("%d", &systemPassword);
	}
	//Functioning loop of the process
	while(1)
	{
		//First lists current users
		printf("\nUsers:\n\n");
		if(numUsers==0)
		{
			printf("No users yet\n\n");
		}
		for(i=0; i<numUsers; i++)
		{
			printf("\nUser %d:\nName: %s\n\n", i, Users[i].userName);
			for(j=0;j<6;j++)
			{
				if(Users[i].hopperAct[j] == true)
				{
					printf("Hopper %d in use\n", j+1);
				}
			}
		}
		//Presents Main menu options
		printf("\nWhat would you like to do?\n1: Select User Menu (followed by user #)\n2: System Settings\n3: Save & Exit\n\n");

		scanf("%d", &menuSelect);

		switch(menuSelect){
			case 1:
				//userMenu unfinnished
				scanf("%d", &menuSelect);
				printf("Selecting User\n");
				Users[menuSelect] = userMenu(Users[menuSelect]);
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