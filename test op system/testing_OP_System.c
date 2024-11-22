// https://www.geeksforgeeks.org/how-to-detect-operating-system-through-a-c-program/
//
// C program to detect Operating System 

#include <stdio.h> 

// Driver Code 
int main() 
{ 

// Checking for windows OS with 
// _WIN32 macro 
#ifdef _WIN32 
	printf("Hey Geek it seems that"
		"you are working on a Windows OS.\n"); 

// Checking for mac OS with 
// __APPLE__ macro 
#elif __APPLE__ 
	printf("Hey Geek it seems that you "
		"are working on a Mac OS.\n"); 

// Checking for linux OS with 
// __linux__ macro 
#elif __linux__ 
	printf("Hey Geek it seems that you "
		"are working on a Linux OS.\n"); 

// Checking for iOS embedded OS with 
// TARGET_OS_EMBEDDED macro 
#elif TARGET_OS_EMBEDDED 
	printf("Hey Geek it seems that you "
		"are working on an iOS embedded OS.\n"); 

// Checking for iOS simulator OS with 
// TARGET_IPHONE_SIMULATOR macro 
#elif TARGET_IPHONE_SIMULATOR 
	printf("Hey Geek it seems that you "
		"are working on an iOS simulator OS.\n"); 

// Checking for iPhone OS with 
// TARGET_OS_IPHONE macro 
#elif TARGET_OS_IPHONE 
	printf("Hey Geek it seems that you "
		"are working on an iPhone OS.\n"); 

// Checking for MAC OS with 
// TARGET_OS_MAC macro 
#elif TARGET_OS_MAC 
	printf("Hey Geek it seems that you "
		"are working on a MAC OS.\n"); 

// Checking for Android OS with 
// __ANDROID__ macro 
#elif__ANDROID__ 
	printf("Hey Geek it seems that you "
		"are working on an android OS.\n"); 

// Checking for unix OS with 
// __unix__ macro 
#elif __unix__ 
	printf("Hey Geek it seems that you "
		"are working on a unix OS.\n"); 

// Checking for POSIX based OS with 
// _POSIX_VERSION macro 
#elif _POSIX_VERSION 
	printf("Hey Geek it seems that you "
		"are working on a POSIX based OS.\n"); 

// Checking for Solaris OS with 
// __sun macro 
#elif __sun 
	printf("Hey Geek it seems that you "
		"are working on a Solaris OS.\n"); 

// Checking for HP UX OS with 
// __hpux macro 
#elif __hpux 
	printf("Hey Geek it seems that you "
		"are working on a HP UX OS.\n"); 

// Checking for BSD OS with 
// BSD macro 
#elif BSD 
	printf("Hey Geek it seems that you "
		"are working on a Solaris OS.\n"); 

// Checking for DragonFly BSD OS with 
// __DragonFly__ macro 
#elif __DragonFly__ 
	printf("Hey Geek it seems that you "
		"are working on a DragonFly BSD OS.\n"); 

// Checking for FreeBSD OS with 
// __FreeBSD__ macro 
#elif __FreeBSD__ 
	printf("Hey Geek it seems that you "
		"are working on a FreeBSD OS.\n"); 

// Checking for Net BSD OS with 
// __NetBSD__ macro 
#elif __NetBSD__ 
	printf("Hey Geek it seems that you "
		"are working on a Net BSD OS.\n"); 

// Checking for Open BSD OS with 
// __OpenBSD__ macro 
#elif __OpenBSD__ 
	printf("Hey Geek it seems that you "
		"are working on an Open BSD OS.\n"); 

// If neither of them is present 
// then this is printed... 
#else 
	printf("Sorry, the system are"
		"not listed above.\n"); 
#endif 
	return 0; 
} 
