
#include <fcntl.h>
#include <wchar.h>
#include <io.h>
#define RED     "\x1b[31m"
#define GREY     "\x1b[90m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[94m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#define normalChar _setmode(_fileno(stdout), _O_TEXT);
#define unicodeChar _setmode(_fileno(stdout), _O_U16TEXT);

extern char *icon[6];
extern char *loginButton[5];
extern char *signinButton[5];
extern char *searchButton[5];
extern char *timelineButton[5];
extern char *profileButton[5];
extern char *settingsButton[5];
extern char *logoutButton[5];
extern char *divider[2];
extern char *profilePics[17][6];
extern char *bioButton[5];
extern char *CPassButton[5];
extern char *SelectButton[5];
extern char *RefreshButton[5];
extern char *TweetButton[5];
extern void printASCIIart(char *art[] , int size , char color , int space);

