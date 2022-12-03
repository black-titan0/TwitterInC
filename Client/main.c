#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include "myGUI.h"
#include "cJSON.h"
#include "myNetwork.h"

#define MESSAGE cJSON_GetStringValue(currentResp.message)
#define picsNum 17

char Page = 'F';

typedef struct
{
    char whoSaid[25];
    char whatIsSaid[140];
}comment;
typedef struct
{
    char author[25];
    char content[140];
    int likes ,id, numberOfComments;
    comment comments[1000];

}tweet;
typedef struct
{
    char username[100];
    char bio[140];
    int followers , followings ,profilePicture , numberOfTweets;
    char amFollowing[15];
    tweet tweets[1000];


}profile;
typedef struct
{
    char type[20];
    cJSON *message;

}Response;

void PrintComment(comment *);
void PrintTweet(tweet *);
void FindOutCurrentResp(char *);
void FindOutPerson(cJSON *);
void GoToPage(char which);
void printASCIIart(char *art[] , int,char,int);
int init(int);
char *Send(char *);
void PrintResponse();







int error = -1 , numberOfCommentPages,numberOfProfilePages ,tweetPageNavigator= 1 ,commentPageNavigator= 1,firstC,lastC, firstTweetToPrint  , lastTweetToPrint ,timeLine =0 ;
Response currentResp;
char username[100] ,id[100],Comment[140];
char userSearch[100];
char pass[100] , newPass[100];
static char Token[100];
char bio[140];
char bioSend[142];
int profPic = 0;
profile profileToShow;
char response[100000000];


int main() {



    GoToPage('F');
    int port = 12345;
    int initStatus = init(port);
    if (initStatus !=0) {printf(RED "Initializing Failed ! \n Please Try Again And Restart The Program" RESET); getchar();}


    while (initStatus == 0)
    {


        if(Page == 'F')//First Page Functionality
        {
            int num;
            scanf("%d", &num);
            if (num == 2) {
                GoToPage('L');

            } else if (num == 1) {
                GoToPage('S');
            }else GoToPage('F');

        }
        if(Page =='S')//Sign In Page Functionality
        {
            char message[100000] = "signup ";
            char *str2 = ", ";



            FindOutCurrentResp( Send(strcat(strcat(strcat(message , username), str2) , pass)));
            if (!(strcmp(currentResp.type , "Successful"))) {error = -2 ;GoToPage('L');}
            else if (!(strcmp(currentResp.type , "Error")))
            {
                if(!(strcmp(MESSAGE , "This username is already taken."))) error =0;

                GoToPage('S');

            } else GoToPage('S');
        }
        if(Page == 'L')//Log In Page Functionality
        {
            char message[100000] = "login ";
            char *str2 = ", ";



            FindOutCurrentResp(Send(strcat(strcat(strcat(message , username), str2) , pass)));
            if (!(strcmp(currentResp.type , "Token"))) {sprintf(Token ,"%s",MESSAGE);GoToPage('M');}
            else if (!(strcmp(currentResp.type , "Error")))
            {
                if(!(strcmp(MESSAGE , "This username is not valid."))) error =0;
                if(!(strcmp(MESSAGE , "Incorrect Password."))) error =1;
                if(!(strncmp(MESSAGE , "The" , 3))) error =2;
                GoToPage('L');

            } else GoToPage('L');

        }

        if(Page == 'M')//Main Menu Page Functionality
        {
            int num;
            scanf("%d", &num);
            if(num == 1) GoToPage('T');
            //else if(num == 2)
            else if(num == 2){GoToPage('H');}
            else if(num == 3){ char message[100000] = "profile "; tweetPageNavigator =1;FindOutCurrentResp(Send(strcat(message , Token)));FindOutPerson(currentResp.message);GoToPage('P');}
            else if(num == 4){GoToPage('I');}
            else if(num == 5){ char message[100000] = "logout "; Send(strcat(message , Token)); error =0;GoToPage('F');}
            else GoToPage('M');


        }
        if(Page == 'I')//Settings Page Functionality
        {
            int num;
            scanf("%d", &num);
            if(num == 1){GoToPage('C');}
            else if(num == 2){GoToPage('W');}

            else if(num == 0){GoToPage('M');}
            else{GoToPage('I');}


        }
        if(Page == 'H')
        {
            char message[100000] = "search ";
            char *str2 = ", ";



            FindOutCurrentResp(Send(strcat(strcat(strcat(message , Token), str2) , userSearch)));

            if(!(strcmp(currentResp.type , "Profile"))){FindOutPerson(currentResp.message); GoToPage('P');}
            else if(!(strcmp(currentResp.type , "Error")))
            {if (!(strcmp(MESSAGE , "This username is not valid."))){error = 0; GoToPage('H');}
             if (!(strcmp(MESSAGE , "Searched username is your username."))){error = 1; GoToPage('H');}
            }else{GoToPage('H');}


        }//Search Page Functionality
        if(Page == 'P')
        {


            int num=0;
            scanf("%*c%c" , &num);
            if(num == 49)
            {

                if (profileToShow.amFollowing[0]=='N')
                {

                    char message[100000] = "follow ";
                    char *str2 = ", ";



                    FindOutCurrentResp(Send(strcat(strcat(strcat(message, Token), str2), userSearch)));
                    char *srtf = "Followed";
                    sprintf(profileToShow.amFollowing,"%s" , srtf );
                    profileToShow.followers++;
                }    else
                    {
                        char message[100000] = "unfollow ";
                        char *str2 = ", ";


                        FindOutCurrentResp(Send(strcat(strcat(strcat(message, Token), str2), userSearch)));
                        char *srtf = "NotFollowed";
                        sprintf(profileToShow.amFollowing,"%s" , srtf );
                        profileToShow.followers--;
                    }

                GoToPage('P');
            }
            else if(num == 48){timeLine =0;tweetPageNavigator = 1; GoToPage('M');}
            else if(num == 60)
            {
                if (tweetPageNavigator!=1) {tweetPageNavigator--; lastTweetToPrint+=4;firstTweetToPrint+=4;}
                GoToPage('P');

            }
            else if(num == 62)
            {
                if (tweetPageNavigator!=numberOfProfilePages)
                {

                if(tweetPageNavigator!=1 || timeLine){lastTweetToPrint-=4;firstTweetToPrint-=4;}
                else {firstTweetToPrint-=1;}
                tweetPageNavigator++;
                }
                GoToPage('P');

            }
            else if(num == 108)
            {
                printf(GREEN "Please Type "RED"ID "GREEN "Of The Tweet You Like:\n" CYAN);
                scanf("%*c%[^\n]s" , id);
                if (!(strcmp(id , "0"))) { GoToPage('P');}
                else
                    {
                        char message[100000] = "like ";
                        char *str2 = ", ";



                        FindOutCurrentResp(Send(strcat(strcat(strcat(message , Token), str2) , id)));

                        if(!(strcmp(currentResp.type , "Successful"))){
                            char str3[6];
                            for (int i = 0; i < profileToShow.numberOfTweets; ++i) {
                                itoa(profileToShow.tweets[i].id , str3 , 10);
                                if(!(strcmp(id , str3 ))){
                                    if(!(strcmp(MESSAGE , "liked."))){
                                        profileToShow.tweets[i].likes++;
                                        error = 0;
                                    }
                                    if(!(strcmp(MESSAGE , "unliked."))) {
                                        profileToShow.tweets[i].likes--;
                                        error = -4;
                                    }

                                }
                            }
                            }else{
                            if(!strcmp(MESSAGE , "tweet not found."))
                                error = 2;
                        }
                        GoToPage('P');
                    }
            }
            else if(num == 100)
            {
                printf(GREEN "Please Type "RED"ID "GREEN "Of The Tweet You Want to " RED"DELETE" ":\n" CYAN);
                scanf("%*c%[^\n]s" , id);
                if (!(strcmp(id , "0"))) { GoToPage('P');}
                else
                {
                    char message[100000] = "delete ";
                    char *str2 = ", ";


                    FindOutCurrentResp(Send(strcat(strcat(strcat(message , Token), str2) , id)));

                    if(!(strcmp(currentResp.type , "Successful"))){
                        error = -5;

                    }
                    else {
                        if(!strcmp(MESSAGE , "tweet not found."))
                            error = 2;
                        else if (!strcmp(MESSAGE , "not yours."))
                            error = 3;
                    }
                    GoToPage('P');}
            }
            else if(num == 99)
            {
                printf(GREEN "Please Type "RED"ID "GREEN "Of The Tweet You Want:\n" CYAN);
                scanf("%*c%[^\n]s" , id);
                if (!(strcmp(id , "0"))) { GoToPage('P');}
                else
                    {printf(GREEN "Now Please Type Your"RED"Comment"GREEN ":\n" CYAN);
                        scanf("%*c%[^\n]s" , Comment);}
                if (!(strcmp(Comment , "0"))) { GoToPage('P');}
                else
                {
                    char message[100000] = "comment ";
                    char *str2 = ", ";



                    FindOutCurrentResp( Send(strcat(strcat(strcat(strcat(strcat(message , Token), str2) , id),str2),Comment)));
                    if(!(strcmp(currentResp.type , "Successful"))){
                        char str3[6];
                        for (int i = 0; i < profileToShow.numberOfTweets; ++i) {
                            itoa(profileToShow.tweets[i].id , str3 , 10);
                            if(!(strcmp(id , str3 ))){
                                profileToShow.tweets[i].numberOfComments++;
                                error = -2;
                            }
                        }

                    }
                    else {
                        if(!strcmp(MESSAGE , "tweet not found."))
                            error = 2;
                    }

                        GoToPage('P');
                }
            }
            else if(num == 115)
            {
                printf(GREEN "Please Type "RED"ID "GREEN "Of The Tweet You Want:\n" CYAN);
                scanf("%*c%[^\n]s" , id);
                if (!(strcmp(id , "0"))) { GoToPage('P');}
                else GoToPage('O');

            }
            else {GoToPage('P');}
        }//Profile Page Functionality
        if (Page == 'W')
        {
            char message[100000] = "changepassword ";
            char *str2 = ", ";


            FindOutCurrentResp( Send(strcat(strcat(strcat(strcat(strcat(message , Token), str2) , pass) , str2),newPass)));
            if (!(strcmp(currentResp.type , "Successful"))) {error = -3;sprintf(pass , "%s" , newPass);GoToPage('I');}
            else GoToPage('I');

        }//Change Password Page Functionality
        if(Page == 'B')
        {
            char message[100000] = "setbio ";
            char *str2 = ", ";
            if(profPic < 10) sprintf(bioSend , "%s" , "0");
            else sprintf(bioSend , "%s" , "");
            char str4[100];
            itoa(profPic , str4 , 10);
            strcat(bioSend,str4);
            char *str3 = strcat(bioSend , bio);


            FindOutCurrentResp(Send(strcat(strcat(strcat(message , Token), str2) , str3)));
            if (!(strcmp(currentResp.type , "Successful"))) {error = -2;GoToPage('I');}
            else if (!(strcmp(currentResp.type , "Error")))
            {
                error = 0;
                GoToPage('I');

            }
            profPic = 0;

        }//Change Bio Page Functionality
        if(Page == 'C')
        {
            int num = 0;
            while ( !(num == 48 || num ==60 || num == 49 || num == 62))
             scanf("%c" , &num);
            if(num == 49)
            {
                GoToPage('B');
            }
            else if(num == 48){profPic = 0; GoToPage('I');}
            else if(num == 60)
            {
                if (profPic!=0) {profPic--;}
                GoToPage('C');

            }
            else if(num == 62)
            {
                if (profPic!= picsNum - 1) {profPic++;}
                GoToPage('C');
            }else GoToPage('C');
        }//Change Avatar Functionality
        if(Page == 'T')
        {
            int numb = 0;
            scanf("%d" , &numb);
            if(numb == 1)
            {
                char message[100000] = "refresh ";
                FindOutCurrentResp(Send(strcat(message , Token)));
                if(!(strcmp(currentResp.type , "List"))){timeLine =1;FindOutPerson(currentResp.message);firstTweetToPrint = profileToShow.numberOfTweets-1;lastTweetToPrint =firstTweetToPrint-3;GoToPage('P');}
            }
            else if(numb == 2){GoToPage('!');}
            else if(numb == 0){GoToPage('M');}
            else GoToPage('T');
        }//TimeLine Page Functionality
        if(Page == '!')
        {
            char message[100000] = "sendtweet ";
            char *str2 = ", ";


            FindOutCurrentResp(Send(strcat(strcat(strcat(message , Token), str2) , Comment)));
            if (!(strcmp(currentResp.type , "Successful"))) {error = -2;GoToPage('T');}
            else if (!(strcmp(currentResp.type , "Error")))
            {

                GoToPage('T');

            }


        }//Change Bio Page Functionality
        if (Page == 'O')
        {
            int num=0;
            scanf("%*c%c" , &num);
            if(num == 48){commentPageNavigator = 1; GoToPage('P');}
            else if(num == 60)
            {
                if (commentPageNavigator!=1) {commentPageNavigator--; lastC+=3;firstC+=3;}
                GoToPage('O');

            }
            else if(num == 62)
            {
                if (commentPageNavigator!=numberOfCommentPages)
                {

                  lastC-=3;firstC-=3;

                    commentPageNavigator++;
                }
                GoToPage('O');

            }
            else if(num == 108)
            {


                    char message[100000] = "like ";
                    char *str2 = ", ";



                    FindOutCurrentResp(Send(strcat(strcat(strcat(message , Token), str2) , id)));

                if(!(strcmp(currentResp.type , "Successful"))){
                    char str3[6];
                    for (int i = 0; i < profileToShow.numberOfTweets; ++i) {
                        itoa(profileToShow.tweets[i].id , str3 , 10);
                        if(!(strcmp(id , str3 ))){
                            if(!(strcmp(MESSAGE , "liked."))){
                                profileToShow.tweets[i].likes++;
                                error = 0;
                            }
                            if(!(strcmp(MESSAGE , "unliked."))) {
                                profileToShow.tweets[i].likes--;
                                error = -4;
                            }

                        }
                    }
                        GoToPage('P');}

            }
            else if(num == 99)
            {



                printf(GREEN "Please Type Your"RED"Comment"GREEN ":\n" CYAN);
                    scanf("%*c%[^\n]s" , Comment);
                if (!(strcmp(Comment , "0"))) { GoToPage('P');}
                else
                {
                    char message[100000] = "comment ";
                    char *str2 = ", ";



                    FindOutCurrentResp(Send(strcat(strcat(strcat(strcat(strcat(message , Token), str2) , id),str2),Comment)));

                    if(!(strcmp(currentResp.type , "Successful"))){
                        char str3[6];
                        for (int i = 0; i < profileToShow.numberOfTweets; ++i) {
                            itoa(profileToShow.tweets[i].id , str3 , 10);
                            if(!(strcmp(id , str3 ))){
                                profileToShow.tweets[i].numberOfComments++;
                                error = -2;
                            }
                        }

                    }
                    else {
                        if(!strcmp(MESSAGE , "tweet not found."))
                            error = 2;
                    }
                }
                GoToPage('P');
            }
            else if(num == 100)
            {

                    char message[100000] = "delete ";
                    char *str2 = ", ";



                    FindOutCurrentResp(Send(strcat(strcat(strcat(message , Token), str2) , id)));

                    if(!(strcmp(currentResp.type , "Successful"))){
                        error = -5;

                    }
                    else {
                        if(!strcmp(MESSAGE , "tweet not found."))
                            error = 2;
                        else if (!strcmp(MESSAGE , "not yours."))
                            error = 3;
                    }
                    GoToPage('O');
            }

        }
    }

    return 0;
}



void GoToPage(char which)
{
    system("mode 45,45");
    Page = which;

    system("cls");


    if(which == 'F')//First Page Printing
    {

        printASCIIart(icon , 6 , 'C' , 2);
        printASCIIart(signinButton , 5 , 'M' , 2);
        printASCIIart(loginButton , 5 , 'M' , 2);
        if(error == 0)printf(GREEN "         YOU" RED " LOGGED OUT " GREEN "SUCCESSFULLY!\n"RESET);
        printASCIIart(divider , 1 , 'Y' , 2);
        printf("Please Choose an Option:");

    }
    if(which == 'L')//Login page printing
    {
        printASCIIart(loginButton , 5 , 'Y' , 2);
        if (error == -2) {printf(GREEN "        YOU SIGNED UP SUCCESSFULLY!\n"RESET);printf(GREEN "            Now Please Log In\n"RESET);}
        if (error == 0) printf(RED "            USERNAME NOT FOUND!!\n" RESET);
        if (error == 1) printf(RED "        YOUR PASSWORD WAS INCORRECT!!\n" RESET);
        if (error == 2) printf(RED "SOMEONE IS ALREADY LOGGED INTO YOU ACCOUNT!!\n" RESET);
        printASCIIart(divider , 1 , 'Y' ,2);
        printf(GREEN "Please Type Your "RED"Username "GREEN "And Hit Enter:\n" CYAN);
        scanf("%*c%[^\n]s" , username);
        if (!(strcmp(username , "0"))) { GoToPage('F'); return;}
        printf(GREEN "Now Please Type Your " RED "Password "GREEN "And Hit Enter:\n" CYAN);
        scanf("%*c%[^\n]s" , pass);
        if (!(strcmp(pass , "0"))) { GoToPage('F');}
    }
    if(which == 'S')//Sign in page printing
    {
        printASCIIart(signinButton , 5 , 'Y',2);
        if (error == 0) printf(RED "           USERNAME ALREADY TAKEN!!\n" RESET);
        printASCIIart(divider, 1 ,'Y',2);
        printf(YELLOW "          Lets Make You An Account!!\n\n" RESET);
        printf(GREEN "Please Type a "RED"NAME "GREEN "You Wish We Call You:\n" CYAN);
        scanf("%*c%[^\n]s" , username);
        if (!(strcmp(username , "0"))) { GoToPage('F'); return;}
        printf(GREEN "Now Please Type Your "RED"Password "GREEN "And Hit Enter:\n" CYAN);
        scanf("%*c%[^\n]s" , pass);
        if (!(strcmp(pass , "0"))) { GoToPage('F');}

    }
    if(which =='M')
    {
        timeLine = 0;
        printASCIIart(divider , 1 , 'Y' ,1);
        int n = (45-(strlen(username) + strlen("||Twitter!|MAIN MENU|")))/6;
        int n2 = (45-(6*n)-(strlen(username) + strlen("||Twitter!|MAIN MENU|")))/2;
        printf(YELLOW"|");
        for(int i = 0 ; i < n ; ++i) printf(" ");
        printf(MAGENTA"%s" , username);
        for(int i = 0 ; i < n ; ++i) printf(" ");
        printf( YELLOW"|");
        for(int i = 0 ; i < n + n2 ; ++i) printf(" ");
        printf(CYAN "Twitter!");
        for(int i = 0 ; i < n + n2 ; ++i) printf(" ");
        printf( YELLOW"|");
        for(int i = 0 ; i < n ; ++i) printf(" ");
        printf( MAGENTA "MAIN MENU");
        for(int i = 0 ; i < n ; ++i) printf(" ");
        printf( YELLOW"|\n");
        printASCIIart(divider , 1 , 'Y',2);
        printASCIIart(timelineButton , 5 , 'M',2);
        printASCIIart(searchButton , 5 , 'M',2);
        printASCIIart(profileButton , 5 , 'M',2);
        printASCIIart(settingsButton , 5 , 'M',2);
        printASCIIart(logoutButton , 5 , 'M',2);

        printASCIIart(divider , 1 , 'Y',2);
        printf("Please Choose an Option:");
    }
    if(which == 'H')//Search Page printing
    {
        printASCIIart(searchButton , 5, 'Y',2);
        if(error == -1)
           printf(GREEN "   Here You Can Find Those You "RED"Love\\Hate "GREEN "\n");
        else if(error == 0)
           printf(RED "             USERNAME NOT FOUND!             \n");
        else if(error == 1)
            printf(RED "    Seriously?Look In The Mirror Not Here!    \n");
        printASCIIart(divider , 1 , 'Y',2);

        printf(GREEN "Just Type In Their "RED"USERNAME "CYAN ":\n");
        scanf("%*c%[^\n]s" , userSearch);
        if (!(strcmp(userSearch , "0"))) { GoToPage('M');}
    }
    if(which == 'P')//Profile Page printing
    {

      if(tweetPageNavigator == 1 && !timeLine)
      {
          firstTweetToPrint = profileToShow.numberOfTweets - 1;
          lastTweetToPrint = ((profileToShow.numberOfTweets - 5) > 0) ?profileToShow.numberOfTweets - 5 :0;
          printASCIIart(divider + 1, 1, 'Y', 2);
          printASCIIart(profilePics[profileToShow.profilePicture], 6, 'C', 2);

          char str[100], str2[100];
          int n = (45 - (strlen("followingsfollowers||::") + strlen(itoa(profileToShow.followings, str, 10)) +
                         strlen(itoa(profileToShow.followers, str2, 10)))) / 4;
          for (int i = 0; i < (45 - strlen(profileToShow.username)) / 2; ++i) printf(" ");
          printf(MAGENTA "%s\n" RESET, profileToShow.username);
          printASCIIart(divider + 1, 1, 'E', 1);
          printf(YELLOW "%s\n", profileToShow.bio + 2);
          printASCIIart(divider + 1, 1, 'E', 1);
          printf(GREY"|" MAGENTA"Followers");
          for (int i = 0; i < n; ++i) printf(" ");
          printf(YELLOW":");
          for (int i = 0; i < n; ++i) printf(" ");
          printf(MAGENTA"%d", profileToShow.followers);
          printf(GREY"|" MAGENTA"Followings");
          for (int i = 0; i < n; ++i) printf(" ");
          printf(YELLOW":");
          for (int i = 0; i < n - 1; ++i) printf(" ");
          printf(MAGENTA"%d" YELLOW "|\n", profileToShow.followings);

          printASCIIart(divider + 1, 1, 'Y', 1);
          if (profileToShow.amFollowing[0] == 'N') {
              for (int i = 0; i < (45 - strlen("1.Follow")) / 2; ++i) printf(" ");

              printf(GREEN"1.Follow\n" RESET);
          } else if (profileToShow.amFollowing[0] == 'F'){
              for (int i = 0; i < (45 - strlen("1.Unfollow")) / 2; ++i) printf(" ");
              printf(RED "1.Unfollow\n" RESET);
          } else if (profileToShow.amFollowing[0] == 'Y'){
              for (int i = 0; i < (45 - strlen("Your Profile")) / 2; ++i) printf(" ");
              printf(YELLOW "Your Profile\n" RESET);
          }
          printASCIIart(divider + 1, 1, 'Y', 3);
          printASCIIart(divider, 1, 'Y', 1);
          printf(CYAN"    <-             TWEETS               ->   \n");
          printASCIIart(divider + 1, 1, 'E', 1);
          if (profileToShow.numberOfTweets !=0)
            PrintTweet(profileToShow.tweets + firstTweetToPrint);
          else
              printf(RED"This User Hasn't Tweeted Yet!\n");
          printASCIIart(divider, 1, 'Y', 1);
          printf(GREY"      <-         [%d / %d]           ->      \n",tweetPageNavigator , numberOfProfilePages);
          printASCIIart(divider, 1, 'Y', 2);
          if (error == 0)
              printf(GREEN"Tweet Liked Successfully!\n");
          else if (error == -4)
              printf(GREEN"Tweet Unliked Successfully!\n");
          else if (error == -5)
              printf(GREEN"Tweet Deleted Successfully!\n");
          else if(error == -2)
              printf(GREEN"Comment Submitted Successfully!\n");
          else if(error == -3)
              printf(GREEN"Tweet Deleted Successfully!\n");
          else if(error == 3)
              printf(RED"You Can't Delete Other's Tweets!\n");
          else if(error == 2)
              printf(RED"Tweet Not Found!\n");
          printASCIIart(divider + 1, 1, 'Y', 2);
          printf("Please Choose an Option:");
      }
      else if(tweetPageNavigator <= numberOfProfilePages)
      {

          for (int i = firstTweetToPrint; (i >= lastTweetToPrint ) && i >=0; --i)
          {
            PrintTweet(profileToShow.tweets+i);
          }
          printASCIIart(divider, 1, 'Y', 1);
          printf(GREY"      <-         [%d / %d]           ->      \n",tweetPageNavigator , numberOfProfilePages);
          printASCIIart(divider, 1, 'Y', 2);
          if (error == 0)
              printf(GREEN"Tweet Liked Successfully!\n");
          else if (error == -4)
              printf(GREEN"Tweet Unliked Successfully!\n");
          else if (error == -5)
              printf(GREEN"Tweet Deleted Successfully!\n");
          else if(error == -2)
              printf(GREEN"Comment Submitted Successfully!\n");
          else if(error == -3)
              printf(GREEN"Tweet Deleted Successfully!\n");
          else if(error == 3)
              printf(RED"You Can't Delete Other's Tweets!\n");
          else if(error == 2)
              printf(RED"Tweet Not Found!\n");
          printASCIIart(divider + 1, 1, 'Y', 2);
          printf("Please Choose an Option:");
      }
    }
    if(which == 'I')//Settings Page printing
    {

        printASCIIart(bioButton, 5 , 'M' , 2);
        printASCIIart(CPassButton , 5 , 'M' , 2);
        if (error == -3 )printf(GREEN"     Your Password Updated Successfully!     \n");
        if (error == -2) printf(GREEN"        Your Bio Updated Successfully!       \n");
        if (error == 0) printf(RED"        You've Already Updated Your Bio!       \n");
        if (error == 1) printf(RED"     You've Entered Current Password Wrong!    \n");
        printASCIIart(divider , 1 , 'Y',2);
        printf("Please Choose an Option:");
    }
    if(which == 'B')//Change Bio page printing
    {
        printASCIIart(bioButton , 5 , 'Y' , 2);
        printASCIIart(divider , 1 , 'Y' ,2);
        printf(GREEN "Please Type The "RED"Bio "GREEN "You'd Like To Share With Others:\n" CYAN);
        scanf("%*c%[^\n]s" , &bio);
        if (!(strcmp(bio , "0"))) { GoToPage('I'); return;}
    }
    if(which == 'W')//Change Password page printing
    {
        printASCIIart(CPassButton , 5 , 'Y' , 2);
        printASCIIart(divider , 1 , 'Y' ,2);
        printf(GREEN "Please Type Your "RED"Current Password"GREEN "To Confirm It's You!:\n" CYAN);
        char str1[100];
        scanf("%*c%[^\n]s" , &str1);
        if (!(strcmp(str1 , "0"))) {GoToPage('I'); return;}
        if(!(strcmp(str1 , pass)))
        {
            printf(GREEN " Now Please Type Your "RED"New Password"GREEN ":\n" CYAN);
            scanf("%*c%[^\n]s" , &newPass);
            if (!(strcmp(newPass , "0"))) {GoToPage('I'); return;}

        }else{error = 1;GoToPage('I');}


    }
    if(which == 'C')//Change Avatar page printing;
    {
        printASCIIart(bioButton , 5 , 'Y' , 2);
        printASCIIart(divider , 1 , 'Y' ,2);
        printASCIIart(profilePics[profPic] , 6 , 'C' , 2);
        for (int i = 0; i < (45 - strlen(username)) / 2; ++i) printf(" ");
        printf(MAGENTA "%s\n" RESET, username);
        printASCIIart(SelectButton, 5 , 'M' , 2);
        printASCIIart(divider, 1, 'Y', 1);
        printf(GREY"      <-         [%d / %d]           ->      \n" , profPic+1 , picsNum);
        printASCIIart(divider, 1, 'Y', 2);
        printf("Please Choose an Option:");

    }
    if(which == 'T')//time line Page printing
    {

        printASCIIart(RefreshButton, 5 , 'M' , 2);
        printASCIIart(TweetButton , 5 , 'M' , 2);

        if (error == -2) printf(GREEN "           Tweet Successfully Sent!          \n");
        if (error == 0) printf(RED "   Tweet Must Be Less Than " YELLOW "140" RED " Characters!   \n");
        printASCIIart(divider , 1 , 'Y',2);
        printf("Please Choose an Option:");

    }
    if(which == '!')//Tweet page printing
    {
        printASCIIart(TweetButton , 5 , 'Y' , 2);
        printASCIIart(divider , 1 , 'Y' ,2);
        printf(GREEN "What Is Going On? :\n" CYAN);
        scanf("%*c%[^\n]s" , &Comment);
        if (!(strcmp(Comment , "0"))) { GoToPage('T'); return;}
        if (strlen(Comment) > 140) {error = 0;GoToPage('T');}
    }
    if(which == 'O')//Comments Page Printing
    {

        tweet * TWEET;
        for (int i =0; i < profileToShow.numberOfTweets ; i++)
        {
            int ID =atoi(id);
            if(profileToShow.tweets[i].id == ID) { TWEET = profileToShow.tweets +i; break; }
        }
        numberOfCommentPages = ceil(((double )(TWEET->numberOfComments))/((double )(4)));
        if(commentPageNavigator == 1){firstC = TWEET->numberOfComments-1; lastC =firstC-2;}
        PrintTweet(TWEET);
        printf(GREY"  <-             Comments               -> \n");
        for (int i = firstC; (i >= lastC ) && i >=0; --i)
        {
            PrintComment(TWEET->comments+i);
        }
        printASCIIart(divider, 1, 'Y', 1);
        printf(GREY"      <-         [%d / %d]           ->      \n",commentPageNavigator , numberOfCommentPages);
        printASCIIart(divider, 1, 'Y', 2);
        if (error == 0)
            printf(GREEN"Tweet Liked Successfully!\n");
        else if (error == -4)
            printf(GREEN"Tweet Unliked Successfully!\n");
        else if (error == -5)
            printf(GREEN"Tweet Deleted Successfully!\n");
        else if(error == -2)
            printf(GREEN"Comment Submitted Successfully!\n");
        else if(error == -3)
            printf(GREEN"Tweet Deleted Successfully!\n");
        else if(error == 3)
            printf(RED"You Can't Delete Other's Tweets!\n");
        else if(error == 2)
            printf(RED"Tweet Not Found!\n");
        printASCIIart(divider + 1, 1, 'Y', 2);
        printf("Please Choose an Option:");

    }

    error = -1;
}

void FindOutCurrentResp(char *message)
{
    cJSON *JSONmessage = cJSON_Parse(message);
    sprintf(currentResp.type ,"%s", cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(JSONmessage , "type")));
    currentResp.message = cJSON_GetObjectItemCaseSensitive(JSONmessage , "message");
    free(message);
}
void FindOutPerson(cJSON *JSONmessage)
{
    memset(profileToShow.bio , 0 , 140);
    struct cJSON *Tweets;

if(!timeLine) {
    sprintf(profileToShow.username, "%s",
            cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(JSONmessage, "username")));
    sprintf(profileToShow.bio, "%s", cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(JSONmessage, "bio")));
    sprintf(profileToShow.amFollowing, "%s",
            cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(JSONmessage, "followStatus")));
    profileToShow.profilePicture =((strcmp(profileToShow.bio ,""))) ?(((int) (profileToShow.bio[0])-48)*10)+((int) (profileToShow.bio[1])-48) :48-48;
    profileToShow.followers = cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(JSONmessage, "numberOfFollowers"));
    profileToShow.followings = cJSON_GetNumberValue(
            cJSON_GetObjectItemCaseSensitive(JSONmessage, "numberOfFollowings"));
    Tweets = cJSON_GetObjectItemCaseSensitive(JSONmessage, "allTweets");
}else{Tweets = JSONmessage;}
    profileToShow.numberOfTweets = cJSON_GetArraySize(Tweets);
    numberOfProfilePages =(timeLine)?(int)ceil(((double )(profileToShow.numberOfTweets))/((double)(4))) :1 + (int)ceil(((double )(profileToShow.numberOfTweets-1))/((double)(4)));
    for (int i = 0; i < profileToShow.numberOfTweets; i++)
    {
        sprintf(profileToShow.tweets[i].author ,"%s",cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(Tweets , i) , "author")));
        sprintf(profileToShow.tweets[i].content ,"%s",cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive( cJSON_GetArrayItem(Tweets , i) , "content")));
        profileToShow.tweets[i].id = cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(Tweets , i) , "id"));
        profileToShow.tweets[i].likes = cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(Tweets , i) , "likes"));
        profileToShow.tweets[i].numberOfComments = cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(Tweets , i) , "comments"));
        for (int j = 0; j < profileToShow.tweets[i].numberOfComments ; ++j)
        {
            sprintf(profileToShow.tweets[i].comments[j].whoSaid,"%s",cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(Tweets , i) , "comments") , j)->string);
            sprintf(profileToShow.tweets[i].comments[j].whatIsSaid,"%s",cJSON_GetStringValue(cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(Tweets , i) , "comments") , j)));
        }
    }


}
void PrintTweet(tweet *tweetToPrint)
{
    printASCIIart(divider +1 ,1 ,'Y' , 1);
    int n = (45-(strlen(tweetToPrint->author) + floor(log10(abs(tweetToPrint->id))) + 1 + strlen("|||")))/6;

    printf(GREY "|"MAGENTA "%s" , tweetToPrint->author );
    for (int i = 0 ; i < 4*n ; ++i) printf(" ");
    printf(GREY "|");
    for (int i = 0 ; i < n ; ++i) printf(" ");
    printf(MAGENTA "%d" , tweetToPrint->id);
    for (int i = 0 ; i < n ; ++i) printf(" ");
    printf(GREY "|\n" RESET);

    printASCIIart(divider +1 , 1 , 'E' , 1);
    printf("%s\n" , tweetToPrint->content);
    printASCIIart(divider +1 , 1 , 'E' , 1);

    n= (45-(floor(log10(abs(tweetToPrint->likes +2))) + 1+floor(log10(abs(tweetToPrint->numberOfComments + 2))) + 1 + 7))/4;
    printf(GREY "|");
    for (int i = 0 ; i < n ; ++i) printf(" ");
    unicodeChar wprintf(RED L"\x2665 "); normalChar
    printf(MAGENTA "%d" , tweetToPrint->likes);
    for (int i = 0 ; i < n ; ++i) printf(" ");
    printf(GREY "|");
    for (int i = 0 ; i < n ; ++i) printf(" ");
    unicodeChar wprintf(CYAN L"\x00a9 "); normalChar
    printf(MAGENTA"%d" , tweetToPrint->numberOfComments);
    for (int i = 0 ; i < n ; ++i) printf(" ");
    printf(GREY "|\n");
    printASCIIart(divider +1 ,1 ,'Y' , 1);
}
void PrintComment(comment *commentToPrint)
{
    printASCIIart(divider +1 ,1 ,'C' , 1);

    printf(GREY "|"MAGENTA "%s" , commentToPrint->whoSaid );
    for (int i = 0 ; i < 43-(strlen(commentToPrint->whoSaid)) ; ++i) printf(" ");
    printf(GREY "|\n");

    printASCIIart(divider +1 , 1 , 'E' , 1);
    printf("%s\n" , commentToPrint->whatIsSaid);


    printASCIIart(divider +1 ,1 ,'C' , 1);
}
