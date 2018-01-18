# 41XX

Functionality
______________________________________________________________________________________________________________________________

Not Implimented

- GUI intended to be designed with control buttons across bottom 1/3 of screen. Arrows on left and right and a select and 
  return button in the middle, totalling 4 buttons. Top 2/3 of screen are for displaying information and occasion keyboards
- Inbounds check for barcode information when adding a user. Will be required in the userMenu Update Barcode section as well

Implimented

-Main Menu backbone. Prompts for a new password if one is not set. (0 counts as the not set value) (Crashes if 0 password 
 and terminate string [1234567890] is not written in save.txt)
	--Text based menu currently displays Users on every visit to Main Menu. Also listes each Hopper used by each user. Allows
	  mixing of hopper between users on purpose.
	--Main Menu options are currently Select User which calls an unfinish userMenu() function, System Settings which lead to 
	  a password wall
		---Password fail falls back to Main Menu
		---Options in System Menu are; AddUser(), ClearUser(); and Factory Defualt (Option seem self explanitory)
		---Options in User Menu are; changeAlarmTime(), changeAlarmTone(), updatePerscription(), userPasswordSettings()
	--Save and exit is final main menu option
- impliment Alarms as seperate tasks which pre-empt all system menus? 
- Prompt will begin dispensing upon agknowledgement
- All other system features hault. Perhaps read from Hardsave to prevent trying to dispense as updating time?
- Dispensing prompts must accumulate. One prompt arriving while another is going off should cause it to wait.


-Hardsave feature. Saves on new users and other changes to settings.
	--Format of save file for reference
	

	Brandon					Name
	1 0 0 0 0 0 			HopperActivation[6] (0 or 1, True or False)
	1 0 0 0 0 0 			HopperSize[6] (We discussed 6-8 possible sizes, so 1-6)
	1 0 0 0 0 0 			HopperNumDisp[6] (Number of pills to be dispensed each serving, 1-3)
	1 0 0 0 0 0 			HopperTimes[6] (Which of the alarms to distribute pills at [Mornin, MidDay, or Evening])
							  (Combos are 1-Mor 2-Mid 3-Eve 4-Mor,Mid 5-Mor,Eve 6-Mid,Eve 7-Mor,Mid,Eve) (1-7)
	1111111 0 0 0 0 0 		HopperDays[6] (Integer string of 1s and 0s representing each day of the week. Each hopper 
							  has a 7 integer number [0 or 1])
	0						UserTone (Integer representing users prefered alarm tone) (1-However Many)
	480 840 1200 			UserAlarms[3] (3 times for users schedules alarms to go off. Integers storing time relative 
							  in minutes. Morning range is 4AM to 12PM, Midday is 12PM to 6PM, Evening is 6PM to 4AM)
							  (Alarms have safe guards to only allow setting at minimum 2 hours apart)
	1						PasswordActivation (Single integer [0 or 1] expressing if the user has chosen to set a password)
	12345 					UserPAssword (Intger storing user password. 5 intger maximum)
	Makayla
	1 0 0 0 0 0 
	2 0 0 0 0 0 
	2 0 0 0 0 0 
	2 0 0 0 0 0 
	1111111 0 0 0 0 0 
	0
	480 840 1200 
	1 
	54321
	1234567890				Termination string that indicates end of load file
