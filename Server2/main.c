#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <time.h>
#include <dirent.h>
#include "cJSON.h"


#define MAX_RES_SIZE 1000000000
#define FILE_CONTENT_SIZE 100000000
typedef struct _Info{
    char field1[1000];
    char field2[1000];
    char field3[1000];
}Info;

typedef struct _ClientOrder{
    char command[20];
    Info information;

}Order;



int initialize(int);
void start(int);
void Log(char * , char);
char * Process(char *);
int SearchForAFile(char *);
char * GenerateToken(char *);
char * ConvertTokenToUsername(char *);
int IsUserOnline(char *);
void selectionSort(int *, int n);

char * response;
int client_socket;
struct sockaddr_in client_adr;
Order currentOrder;
char * print ;


int main() {


    Log("Initializing..." , 'S');
    int server_socket = initialize(12345);
    if(server_socket != INVALID_SOCKET) {
        Log("Socket Initialized!" , 'S');
        Log("Starting Server..." , 'S');
        mkdir("Res");
        mkdir(("Res\\Tweets"));
        FILE * fp = fopen(".\\Res\\Tokens.json" , "w");
        fprintf(fp , "{}");
        fclose(fp);
        start(server_socket);
        cJSON_free(print);
    }
    getchar();
    return 0;
}









int initialize(int port) {
    WSADATA wsa_data;
    int wsa_res = WSAStartup(MAKEWORD(2,2) , &wsa_data);
    if (wsa_res !=0){
        printf("failed to run wsa startup!\n");
        return 0;
    }

    int server_socket = socket(AF_INET , SOCK_STREAM,0);
    if(server_socket == INVALID_SOCKET) {
        printf("Could not create socket!\n");
        return 0;

    }
    struct sockaddr_in server_adr;
    server_adr.sin_family = AF_INET;
    server_adr.sin_addr.s_addr = INADDR_ANY;
    server_adr.sin_port = htons(port);

    int bind_res = bind(server_socket ,(struct sockaddr_in*)(&server_adr) ,sizeof (server_adr));
    if(bind_res ==SOCKET_ERROR) {
        Log("Bind Failed!" , 'E');
        return INVALID_SOCKET;
    }


    listen(server_socket,SOMAXCONN);



    return server_socket;
}

void start(int server_socket) {
    int len = sizeof(struct sockaddr_in);



    char * messages[1000];
    int size = 0;
    Log("Listening on port 12345..." , 'S');
    while ((client_socket = accept(server_socket , (struct  sockaddr_in*)&client_adr , &len))
            != INVALID_SOCKET) {

        response = malloc(MAX_RES_SIZE);
        memset(response , 0 , MAX_RES_SIZE);
        char  *buffer;
        buffer = malloc(MAX_RES_SIZE);
        memset(buffer , 0 ,4000);
        recv(client_socket, buffer ,3999 ,0);
        Log(buffer , 'D');
       sscanf( Process(buffer) ,"%[^\0]s" , response);
        send(client_socket,response, strlen(response) , 0);
        Log(response , 'O');
        free(response);


    }
}

void Log(char * data , char type)
{
    time_t t;
    struct tm* ptm;
    t =time(NULL);
    ptm =localtime(&t);
    char cur[128];
    if(type == 'E') printf("Error! | ");
    else if(type == 'D')printf("Data Entry From Client | ");
    else if(type == 'S')printf("Server Operation | ");
    else if(type == 'O')printf("Response Sent To Client | ");
    strftime(cur , 128 ,"%Y-%b-%d %H:%M:%S | ",ptm);
    printf("%s" ,cur);
    if (type == 'O') printf("\n");
    printf(" %s\n\n" , data);


}


void ParseClientOrder(char * data , int numberOfFields) {


    memset(currentOrder.information.field1 , 0 , 1000);
    memset(currentOrder.information.field2 , 0 , 1000);
    memset(currentOrder.information.field3 , 0 , 1000);

    if(numberOfFields == 1){
        sprintf(currentOrder.information.field1, "%s" , data);
        //printf("field is : %s\n" , currentOrder.information.field1);
        return;
    }else {
        for (int i = 0; *(data) != ','; i++) {
            *(currentOrder.information.field1 + i) = *(data);
            data++;
        }
        data+= 2;
    }

    if(numberOfFields == 2){
        sprintf(currentOrder.information.field2,"%s" , data);
        //printf("field is : %s\n" , currentOrder.information.field1);
      //  printf("field is : %s\n" , currentOrder.information.field2);
        return;
    }else {
        for (int i = 0; *(data) != ','; i++) {
            *(currentOrder.information.field2 + i) = *(data);
            data++;
        }
        data += 2;
    }
    if(numberOfFields == 3)
    sprintf(currentOrder.information.field3, "%s" , data);
    //printf("field is : %s\n" , currentOrder.information.field1);
    //printf("field is : %s\n" , currentOrder.information.field2);
    //printf("field is : %s\n" , currentOrder.information.field3);

    return;





}
char * SignUp() {
    char fileName[50];
    cJSON * theResponse = cJSON_CreateObject();
    cJSON * theProfile = cJSON_CreateObject();
    cJSON * type , * message , * username , * password , * bio ,* followers , *followings , *personalTweets , * last;

    sprintf(fileName , "%s" , currentOrder.information.field1);
    strcat(fileName , ".json");


    if(SearchForAFile(fileName)){
        type = cJSON_CreateString("Error");
        message = cJSON_CreateString("This username is already taken.");
        cJSON_AddItemToObject(theResponse ,"type" ,type);
        cJSON_AddItemToObject(theResponse ,"message" ,message);
        Log("Sign Up Request Failed Because The Requested Username Was Taken!" , 'E');

        return print = cJSON_Print(theResponse);
    } else {
        type = cJSON_CreateString("Successful");
        message = cJSON_CreateString("");
        cJSON_AddItemToObject(theResponse ,"type" ,type);
        cJSON_AddItemToObject(theResponse ,"message" ,message);
        username = cJSON_CreateString(currentOrder.information.field1);
        password = cJSON_CreateString(currentOrder.information.field2);
        bio = cJSON_CreateString("00");
        followers = cJSON_CreateArray();
        followings = cJSON_CreateArray();
        personalTweets = cJSON_CreateArray();
        last = cJSON_CreateNumber(0);
        cJSON_AddItemToObject(theProfile , "username" , username);
        cJSON_AddItemToObject(theProfile , "password" , password);
        cJSON_AddItemToObject(theProfile , "bio" , bio);
        cJSON_AddItemToObject(theProfile , "followers" , followers);
        cJSON_AddItemToObject(theProfile , "followings" , followings);
        cJSON_AddItemToObject(theProfile , "personalTweets" , personalTweets);
        cJSON_AddItemToObject(theProfile , "lastSeen" , last);
        FILE *fp;
        char filePath[100];
        sprintf(filePath , ".\\Res\\%s" , fileName);
        fp = fopen(filePath , "w");
        fprintf(fp, "%s" , print = cJSON_Print(theProfile));
        cJSON_free(print);
        fclose(fp);
        Log("New User Successfully Signed Up!" , 'S');
        return print = cJSON_Print(theResponse);
    }
}
char * LogIn() {
    char fileName[50];
    cJSON * theResponse = cJSON_CreateObject();
    cJSON * type , * message;

    sprintf(fileName , "%s" , currentOrder.information.field1);
    strcat(fileName , ".json");



    if(!SearchForAFile(fileName)){
        type = cJSON_CreateString("Error");
        message = cJSON_CreateString("This username is not valid.");
        Log("Log In Request Failed Because The Requested Username Was Not Found!" , 'E');
    } else {
        if(IsUserOnline(currentOrder.information.field1)) {
            type = cJSON_CreateString("Error");
            message = cJSON_CreateString("The account is already online");
            cJSON_AddItemToObject(theResponse ,"type" ,type);
            cJSON_AddItemToObject(theResponse ,"message" ,message);
            return print = cJSON_Print(theResponse);
        }



        FILE * fp;
        char filePath[100];
        sprintf(filePath , ".\\Res\\%s" , fileName);
        fp = fopen(filePath , "r");
        char * fileContent;
        fileContent = (char *)malloc(FILE_CONTENT_SIZE);
        memset(fileContent , 0 , FILE_CONTENT_SIZE);
        fscanf(fp , "%[^\0]s" , fileContent);
        fclose(fp);
        cJSON * profile = cJSON_Parse(fileContent);
        if(!strcmp(currentOrder.information.field2 ,
                   cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(profile , "password")))) {

            char token[33];
            GenerateToken(token);
            if(strlen(token) != 32) {
                type = cJSON_CreateString("Error");
                message = cJSON_CreateString("Server Failure");
                Log("Server Failed To Generate A Unique Token!" , 'E');
            } else{

                type = cJSON_CreateString("Token");
                message = cJSON_CreateString(token);
                Log("User Logged In Successfully!" , 'S');
            }

        }else {
            type = cJSON_CreateString("Error");
            message = cJSON_CreateString("Incorrect Password.");
            Log("Log In Request Failed Because Password Was Wrong!" , 'E');
        }

        free(fileContent);

    }
    cJSON_AddItemToObject(theResponse ,"type" ,type);
    cJSON_AddItemToObject(theResponse ,"message" ,message);

    return print = cJSON_Print(theResponse);
}
char * LogOut() {
    char user[50];
    sprintf( user , "%s", ConvertTokenToUsername(currentOrder.information.field1));
    cJSON * theResponse = cJSON_CreateObject();
    cJSON * type , * message;

    FILE * fp;
    fp = fopen(".\\Res\\Tokens.json" , "r");
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    cJSON * tokenList = cJSON_Parse(fileContent);
    cJSON_DeleteItemFromObject(tokenList,user);
    fp = fopen(".\\Res\\Tokens.json" , "w");
    fprintf(fp, "%s" , print = cJSON_Print(tokenList));
    cJSON_free(print);
    fclose(fp);

    type = cJSON_CreateString("Successful");
    message = cJSON_CreateString("");
    cJSON_AddItemToObject(theResponse ,"type" ,type);
    cJSON_AddItemToObject(theResponse ,"message" ,message);
    Log("user successfully logged out", 'S');

    free(fileContent);
    return print = cJSON_Print(theResponse);
}
char * Tweet() {
    long long file_count = 0;
    DIR * dirp;
    struct dirent * dir;
    dirp = opendir(".\\Res\\Tweets"); /* There should be error handling after this */
    while ((dir =readdir(dirp) )!= NULL) {
        int n = atoi(dir->d_name);
        if(n != NULL){
            if (n > file_count){
                file_count = n;
            }
        }
    }
    closedir(dirp);

    char user[50] ,fileName[50];
    sprintf( user , "%s", ConvertTokenToUsername(currentOrder.information.field1));
    sprintf( fileName , "%lld",file_count +1);
    strcat(fileName , ".json");
    cJSON * tweet = cJSON_CreateObject();
    cJSON * theResponse = cJSON_CreateObject();
    cJSON * type , *message ,* id , * author , * content , * comments , * likes;
    id = cJSON_CreateNumber(file_count + 1 );
    author = cJSON_CreateString(user);
    content = cJSON_CreateString(currentOrder.information.field2);
    comments = cJSON_CreateObject();
    likes = cJSON_CreateArray();
    cJSON_AddItemToObject(tweet , "id" , id);
    cJSON_AddItemToObject(tweet , "author" , author);
    cJSON_AddItemToObject(tweet , "content" , content);
    cJSON_AddItemToObject(tweet , "comments" , comments);
    cJSON_AddItemToObject(tweet , "likes" , likes);
    FILE *fp;
    char filePath[100];
    sprintf(filePath , ".\\Res\\Tweets\\%s" , fileName);
    fp = fopen(filePath , "w");
    fprintf(fp, "%s" ,print = cJSON_Print(tweet));
    cJSON_free(print);
    fclose(fp);
    type = cJSON_CreateString("Successful");
    message = cJSON_CreateString("You Successfully Tweeted.");
    cJSON_AddItemToObject(theResponse , "type",type);
    cJSON_AddItemToObject(theResponse , "message",message);
    sprintf(filePath , ".\\Res\\%s.json" , user);
    fp = fopen(filePath , "r");
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    cJSON * profile = cJSON_Parse(fileContent);
    cJSON_AddItemToArray(cJSON_GetObjectItemCaseSensitive(profile , "personalTweets") , cJSON_CreateNumber(file_count+1));

    fp = fopen(filePath , "w");
    fprintf(fp , "%s" , print = cJSON_Print(profile));
    cJSON_free(print);
    fclose(fp);
    Log("User Successfully Tweeted!" , 'S');
    free(fileContent);
    return print = cJSON_Print(theResponse);


}
char * Profile(int state) {
    char followStatusS[20] = "NotFollowed";
    cJSON * profile = cJSON_CreateObject();
    cJSON * theResponse = cJSON_CreateObject();
    cJSON * userName , * bio ,* numberOfFollowers, * numberOfFollowings , *allTweets , *message , * type , * followStatus;

    allTweets = cJSON_CreateArray();
    char user[50] , searched[50];
    sprintf(user , "%s" , ConvertTokenToUsername(currentOrder.information.field1));

    if(state == 0) {
        sprintf(searched, "%s", user);
        sprintf(followStatusS , "%s" , "Yourself");
    } else
        sprintf(searched , "%s" , currentOrder.information.field2);

    if(state == 1 && !strcmp(user , searched)){
        type = cJSON_CreateString("Error");
        message = cJSON_CreateString("Searched username is your username.");
        cJSON_AddItemToObject(theResponse , "type" , type);
        cJSON_AddItemToObject(theResponse , "message" , message);

        return print = cJSON_Print(theResponse);
    }
    char fileName[60];
    sprintf(fileName , "%s.json" , searched);
    if(!SearchForAFile(fileName)){
        type = cJSON_CreateString("Error");
        message = cJSON_CreateString("This username is not valid.");
        cJSON_AddItemToObject(theResponse , "type" , type);
        cJSON_AddItemToObject(theResponse , "message" , message);

        return print = cJSON_Print(theResponse);
    }
    char path[100];
    sprintf(path , ".\\Res\\%s" , fileName);
    FILE * fp;
    fp = fopen(path , "r");
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    printf("%s\n" , fileContent);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    cJSON * profileJSON = cJSON_Parse(fileContent);
    userName = cJSON_CreateString(cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(profileJSON , "username")));
    bio = cJSON_CreateString(cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(profileJSON , "bio")));
    numberOfFollowers = cJSON_CreateNumber(cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(profileJSON , "followers")));
    numberOfFollowings = cJSON_CreateNumber(cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(profileJSON , "followings")));
    int number =cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(profileJSON , "personalTweets"));

    for (int i = 0; i < number; ++i) {
        memset(path , 0 , 100);
        memset(fileContent , 0 , 1000000);

        int tweetNum = cJSON_GetNumberValue(cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(profileJSON , "personalTweets") , i));
        sprintf(path , ".\\Res\\Tweets\\%d.json" , tweetNum);
        fp = fopen(path , "r");
        fscanf(fp , "%[^\0]s" , fileContent);
        cJSON * TWEET = cJSON_Parse(fileContent);
        cJSON_AddItemToArray(allTweets , TWEET);
    }


    number = cJSON_GetNumberValue(numberOfFollowers);
    for (int i = 0; i < number; ++i) {
        if(!strcmp(user , cJSON_GetStringValue(cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(profileJSON , "followers"), i))))
            sprintf(followStatusS , "Followed");
    }

    followStatus = cJSON_CreateString(followStatusS);

    cJSON_AddItemToObject(profile , "username" , userName);
    cJSON_AddItemToObject(profile , "bio" , bio);
    cJSON_AddItemToObject(profile , "numberOfFollowers" , numberOfFollowers);
    cJSON_AddItemToObject(profile , "numberOfFollowings" , numberOfFollowings);
    cJSON_AddItemToObject(profile , "followStatus" , followStatus);
    cJSON_AddItemToObject(profile , "allTweets" , allTweets);

    type = cJSON_CreateString("Profile");

    cJSON_AddItemToObject(theResponse , "type" , type);
    cJSON_AddItemToObject(theResponse , "message" , profile);

    free(fileContent);
    return print = cJSON_Print(theResponse);
}
char * Follow() {
    char fileName[50];
    cJSON * theResponse = cJSON_CreateObject();
    cJSON * type , * message;

    sprintf(fileName , "%s" , ConvertTokenToUsername(currentOrder.information.field1));
    FILE * fp;
    char filePath[100];
    sprintf(filePath , ".\\Res\\%s.json" , fileName);
    fp = fopen(filePath , "r");
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    cJSON * profile = cJSON_Parse(fileContent);

    cJSON_AddItemToArray(cJSON_GetObjectItemCaseSensitive(profile , "followings") , cJSON_CreateString(currentOrder.information.field2));
    fp = fopen(filePath , "w");
    fprintf(fp , "%s" ,print = cJSON_Print(profile));
    cJSON_free(print);
    fclose(fp);
    sprintf(filePath , ".\\Res\\%s.json" , currentOrder.information.field2);
    fp = fopen(filePath , "r");
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    profile = cJSON_Parse(fileContent);
    cJSON_AddItemToArray(cJSON_GetObjectItemCaseSensitive(profile , "followers") , cJSON_CreateString(fileName));
    fp = fopen(filePath , "w");
    fprintf(fp , "%s" ,print = cJSON_Print(profile));
    cJSON_free(print);
    fclose(fp);

    type = cJSON_CreateString("Successful");
    message = cJSON_CreateString("");

    cJSON_AddItemToObject(theResponse,"type" , type);
    cJSON_AddItemToObject(theResponse,"message" , message);

    free(fileContent);
    return print = cJSON_Print(theResponse);
}
char * UnFollow() {
    char fileName[50];
    cJSON * theResponse = cJSON_CreateObject();
    cJSON * type , * message;

    sprintf(fileName , "%s" , ConvertTokenToUsername(currentOrder.information.field1));
    FILE * fp;
    char filePath[100];
    sprintf(filePath , ".\\Res\\%s.json" , fileName);
    fp = fopen(filePath , "r");
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    cJSON * profile = cJSON_Parse(fileContent);

    int which = 0 , num;
    num = cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(profile , "followings"));

    for (int i = 0; i < num; ++i) {
        if(!strcmp(cJSON_GetStringValue(cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(profile , "followings") , i)) , currentOrder.information.field2))
            which = i;
    }


    cJSON_DeleteItemFromArray(cJSON_GetObjectItemCaseSensitive(profile , "followings") , which);
    fp = fopen(filePath , "w");
    fprintf(fp , "%s" , print = cJSON_Print(profile));
    cJSON_free(print);
    fclose(fp);
    sprintf(filePath , ".\\Res\\%s.json" , currentOrder.information.field2);
    fp = fopen(filePath , "r");
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    profile = cJSON_Parse(fileContent);
    which = 0;
    num = cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(profile , "followers"));
    for (int i = 0; i < num; ++i) {
        if(!strcmp(cJSON_GetStringValue(cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(profile , "followers") , i)) , fileName))
            which = i;
    }
    cJSON_DeleteItemFromArray(cJSON_GetObjectItemCaseSensitive(profile , "followers") ,which);
    fp = fopen(filePath , "w");
    fprintf(fp , "%s" , print = cJSON_Print(profile));
    cJSON_free(print);
    fclose(fp);

    type = cJSON_CreateString("Successful");
    message = cJSON_CreateString("");

    cJSON_AddItemToObject(theResponse ,"type" ,type);
    cJSON_AddItemToObject(theResponse , "message",message);

    free(fileContent);
    return print = cJSON_Print(theResponse);
}
char * SetBio() {
    cJSON * profile , * type , * message , *theResponse = cJSON_CreateObject();
    char path[100];
    sprintf(path , ".\\Res\\%s.json" , ConvertTokenToUsername(currentOrder.information.field1));
    FILE * fp = fopen(path , "r");
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\n]s" , fileContent);
    fclose(fp);
    profile = cJSON_Parse(fileContent);
    cJSON_SetValuestring(cJSON_GetObjectItemCaseSensitive(profile , "bio") , currentOrder.information.field2);
    fp = fopen(path , "w");
    fprintf(fp , "%s" , print = cJSON_Print(profile));
    cJSON_free(print);
    fclose(fp);

    type = cJSON_CreateString("Successful");
    message = cJSON_CreateString("bio and avatar changed");

    cJSON_AddItemToObject(theResponse , "type" , type);
    cJSON_AddItemToObject(theResponse , "message" , message);

    free(fileContent);
    return print = cJSON_Print(theResponse);


}
char * Refresh(int mode) {
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    cJSON * theResponse = cJSON_CreateObject();
    int last;
    int * tweets;
    tweets = (int *)malloc(5000 * sizeof(int));
    memset(tweets , 0 , 5000);
    cJSON * LIST  = cJSON_CreateArray();

    if(mode == 0) last = 0;
    char path[100];
    sprintf(path , ".\\Res\\%s.json" , ConvertTokenToUsername(currentOrder.information.field1));
    cJSON * followings , *profile;
    FILE * fp;
    fp = fopen(path , "r");
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    profile = cJSON_Parse(fileContent);
    followings = cJSON_GetObjectItemCaseSensitive( profile, "followings");
    if(mode == 1) last = cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(profile , "lastSeen"));
    int num = cJSON_GetArraySize(followings);
    int index = 0 , big = 0;

    for (int i = 0; i < num; ++i) {

        cJSON_free(profile);
        memset(fileContent , 0 , FILE_CONTENT_SIZE);
        sprintf(path , ".\\Res\\%s.json" , cJSON_GetStringValue(cJSON_GetArrayItem(followings , i)));
        fp = fopen(path , "r");
        fscanf(fp ,"%[^\0]s" ,fileContent);
        fclose(fp);
        profile = cJSON_Parse(fileContent);
        int n = cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(profile , "personalTweets"));
        for (int j = 0; j < n; ++j) {
            int tweetID = cJSON_GetNumberValue(cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(profile , "personalTweets") , j));
            if(tweetID > last) {
                if (tweetID > big) big = tweetID;

                tweets[index] = tweetID;
                index ++;
            }
        }

    }
    cJSON_free(profile);
    selectionSort(tweets , index);
    sprintf(path , ".\\Res\\%s.json" , ConvertTokenToUsername(currentOrder.information.field1));
    fp = fopen(path , "r");
    fscanf(fp ,"%[^\0]s" ,fileContent);
    fclose(fp);
    profile = cJSON_Parse(fileContent);
    cJSON_SetNumberValue(cJSON_GetObjectItemCaseSensitive(profile , "lastSeen") , big );
    fp = fopen(path , "w");
    fprintf(fp , "%s" ,print = cJSON_Print(profile));
    fclose(fp);
    cJSON_free(print);

    for (int i = 0; i <index; i++) {
        sprintf(path , ".\\Res\\Tweets\\%d.json" , tweets[i]);
        memset(fileContent , 0 , FILE_CONTENT_SIZE);
        fp = fopen(path , "r");
        fscanf(fp , "%[^\0]s" , fileContent);
        fclose(fp);
        cJSON * t = cJSON_Parse(fileContent);
        cJSON_AddItemToArray(LIST,t);

    }

    cJSON_AddItemToObject(theResponse , "type" , cJSON_CreateString("List"));
    cJSON_AddItemToObject(theResponse , "message" , LIST);

    print = cJSON_Print(theResponse);

    cJSON_free(profile);
    cJSON_free(theResponse);

    free(fileContent);
    return print;




}
char * Like() {

    cJSON * theResponse = cJSON_CreateObject() , * message , * type , *tweet;
    char path[100];
    char user[50];

    sprintf(user , "%s" , ConvertTokenToUsername(currentOrder.information.field1));
    sprintf(path , ".\\Res\\Tweets\\%s.json" ,currentOrder.information.field2);
    FILE * fp = fopen(path , "r");
    if(fp == NULL) {
        type = cJSON_CreateString("Error");
        message = cJSON_CreateString("tweet not found.");
        cJSON_AddItemToObject(theResponse , "type",type);
        cJSON_AddItemToObject(theResponse , "message" , message);
        Log("Tweet Not Found", "E");
        return print = cJSON_Print(theResponse);

    }
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    tweet =cJSON_Parse(fileContent);

    int n = cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(tweet , "likes"));

    for (int i = 0; i < n; ++i) {
        if(!strcmp(user , cJSON_GetStringValue(cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(tweet , "likes"),i)))) {
            cJSON_DeleteItemFromArray(cJSON_GetObjectItemCaseSensitive(tweet , "likes") , i);
            fp = fopen(path , "w");
            fprintf(fp ,"%s" , print = cJSON_Print(tweet));
            fclose(fp);
            cJSON_free(print);
            type = cJSON_CreateString("Successful");
            message = cJSON_CreateString("unliked.");
            cJSON_AddItemToObject(theResponse , "type",type);
            cJSON_AddItemToObject(theResponse , "message" , message);
            Log("Tweet Successfully Unliked", "S");
            free(fileContent);
            return print = cJSON_Print(theResponse);
        }

    }

    cJSON_AddItemToArray(cJSON_GetObjectItemCaseSensitive(tweet , "likes") , cJSON_CreateString(user));
    fp = fopen(path , "w");
    fprintf(fp ,"%s" , print = cJSON_Print(tweet));
    fclose(fp);
    cJSON_free(print);

    type = cJSON_CreateString("Successful");
    message = cJSON_CreateString("liked.");
    Log("Tweet Successfully Liked", "S");

    cJSON_AddItemToObject(theResponse , "type",type);
    cJSON_AddItemToObject(theResponse , "message" , message);
    free(fileContent);
    return print = cJSON_Print(theResponse);

}
char * Comment() {

    cJSON * theResponse = cJSON_CreateObject() , * message , * type , *tweet;
    char path[100];
    char user[50];

    sprintf(user , "%s" , ConvertTokenToUsername(currentOrder.information.field1));
    sprintf(path , ".\\Res\\Tweets\\%s.json" ,currentOrder.information.field2);
    FILE * fp = fopen(path , "r");
    if(fp == NULL) {
        type = cJSON_CreateString("Error");
        message = cJSON_CreateString("tweet not found.");
        cJSON_AddItemToObject(theResponse , "type",type);
        cJSON_AddItemToObject(theResponse , "message" , message);
        Log("Tweet Not Found", "E");
        return print = cJSON_Print(theResponse);

    }
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    tweet =cJSON_Parse(fileContent);





    cJSON_AddItemToObject(cJSON_GetObjectItemCaseSensitive(tweet , "comments") , user,cJSON_CreateString(currentOrder.information.field3));
    fp = fopen(path , "w");
    fprintf(fp ,"%s" , print = cJSON_Print(tweet));
    fclose(fp);
    cJSON_free(print);

    type = cJSON_CreateString("Successful");
    message = cJSON_CreateString("commented.");
    Log("Comment Successfully Submited", "S");

    cJSON_AddItemToObject(theResponse , "type",type);
    cJSON_AddItemToObject(theResponse , "message" , message);
    free(fileContent);
    return print = cJSON_Print(theResponse);

}
char * Delete() {

    cJSON * theResponse = cJSON_CreateObject() , * message , * type , *tweet;
    char path[100];
    char user[50];

    sprintf(user , "%s" , ConvertTokenToUsername(currentOrder.information.field1));
    sprintf(path , ".\\Res\\Tweets\\%s.json" ,currentOrder.information.field2);
    FILE * fp = fopen(path , "r");
    if(fp == NULL) {
        type = cJSON_CreateString("Error");
        message = cJSON_CreateString("tweet not found.");
        cJSON_AddItemToObject(theResponse , "type",type);
        cJSON_AddItemToObject(theResponse , "message" , message);
        Log("Tweet Not Found", "E");
        return print = cJSON_Print(theResponse);

    }
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    tweet =cJSON_Parse(fileContent);
    free(fileContent);
    if(strcmp(cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(tweet , "author")) , user)) {

        type = cJSON_CreateString("Error");
        message = cJSON_CreateString("not yours.");
        cJSON_AddItemToObject(theResponse, "type", type);
        cJSON_AddItemToObject(theResponse, "message", message);
        Log("User Attempted To Delete Other User's Tweet", "E");
        return print = cJSON_Print(theResponse);
    }

    _fcloseall();
    remove(path);
    sprintf(path , ".\\Res\\%s.json" , user);
    fp = fopen(path , "r");
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    cJSON * profile = cJSON_Parse(fileContent);
    int id = atoi(currentOrder.information.field2);
    int n = cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(profile,"personalTweets"));
    for (int i = 0; i < n; ++i) {
       if(id == cJSON_GetNumberValue(cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(profile,"personalTweets"),i))){
           cJSON_DeleteItemFromArray(cJSON_GetObjectItemCaseSensitive(profile,"personalTweets"),i);
       }
    }
    fp = fopen(path , "w");
    fprintf(fp , "%s" , print = cJSON_Print(profile));
    fclose(fp);
    cJSON_free(print);




    type = cJSON_CreateString("Successful");
    message = cJSON_CreateString("deleted.");
    Log("Tweet Deleted Successfully", "S");

    cJSON_AddItemToObject(theResponse , "type",type);
    cJSON_AddItemToObject(theResponse , "message" , message);
    free(fileContent);
    return print = cJSON_Print(theResponse);

}
char * ChangePass() {
    char path[100];
    cJSON * theResponse = cJSON_CreateObject();
    cJSON * type , * message;

    sprintf(path , ".\\Res\\%s.json" , ConvertTokenToUsername(currentOrder.information.field1));

        FILE * fp;
        fp = fopen(path , "r");
        char * fileContent;
        fileContent = (char *)malloc(FILE_CONTENT_SIZE);
        memset(fileContent , 0 , FILE_CONTENT_SIZE);
        fscanf(fp , "%[^\0]s" , fileContent);
        fclose(fp);
        cJSON * profile = cJSON_Parse(fileContent);
        if(!strcmp(currentOrder.information.field2 ,
                   cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(profile , "password")))) {

                cJSON_SetValuestring(cJSON_GetObjectItemCaseSensitive(profile , "password") , currentOrder.information.field3);
                fp = fopen(path , "w");
                fprintf(fp , "%s" , print = cJSON_Print(profile));
                fclose(fp);
                cJSON_free(print);
                type = cJSON_CreateString("Successful");
                message = cJSON_CreateString("Password Updated Successfully!");
                Log("Password Updated Successfully!" , 'S');


        }else {
            type = cJSON_CreateString("Error");
            message = cJSON_CreateString("Incorrect Password.");
            Log("Change Password Request Failed Because Password Was Wrong!" , 'E');
        }

        free(fileContent);


    cJSON_AddItemToObject(theResponse ,"type" ,type);
    cJSON_AddItemToObject(theResponse ,"message" ,message);

    return print = cJSON_Print(theResponse);
}
char * Process(char * data) {


    memset(currentOrder.command , 0,20);
    for (int i = 0; *(data)!=' ' ;i++) {
        *(currentOrder.command + i) = *(data);
        data++;
    }
    data++;
    //printf("command is : %s\n" , currentOrder.command);

    if(!strcmp(currentOrder.command ,"signup")){
        ParseClientOrder(data , 2); return SignUp();}
    else if(!strcmp(currentOrder.command ,"login")){
         ParseClientOrder(data , 2); return LogIn();}
    else if(!strcmp(currentOrder.command ,"logout")){
         ParseClientOrder(data , 1); return LogOut();}
    else if(!strcmp(currentOrder.command ,"sendtweet")){
         ParseClientOrder(data , 2); return Tweet();}
    else if(!strcmp(currentOrder.command ,"search")){
        ParseClientOrder(data , 2); return Profile(1);}
    else if(!strcmp(currentOrder.command ,"profile")){
        ParseClientOrder(data , 1); return Profile(0);}
    else if(!strcmp(currentOrder.command ,"follow")){
        ParseClientOrder(data , 2); return Follow();}
    else if(!strcmp(currentOrder.command ,"unfollow")){
        ParseClientOrder(data , 2); return UnFollow();}
    else if(!strcmp(currentOrder.command ,"setbio")){
        ParseClientOrder(data , 2); return SetBio();}
    else if(!strcmp(currentOrder.command ,"refresh")){
        ParseClientOrder(data , 1); return Refresh(1);}
    else if(!strcmp(currentOrder.command ,"like")){
        ParseClientOrder(data , 2); return Like();}
    else if(!strcmp(currentOrder.command ,"changepassword")){
        ParseClientOrder(data , 3); return ChangePass();}
    else if(!strcmp(currentOrder.command ,"comment")){
        ParseClientOrder(data , 3); return Comment();}
    else if(!strcmp(currentOrder.command ,"delete")){
        ParseClientOrder(data , 2); return Delete();}
    else
    {
        cJSON * theResponse = cJSON_CreateObject();
        cJSON * message , * type;
        type = cJSON_CreateString("Error");
        message = cJSON_CreateString("Bad Request Format.");
        cJSON_AddItemToObject(theResponse , "type" , type);
        cJSON_AddItemToObject(theResponse , "message" , message);

        return print = cJSON_Print(theResponse);
    }

}
int SearchForAFile(char * name)
{
    //printf("searching for %s" , name);
    DIR *d;
    struct dirent *dir;
    d = opendir("./Res");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(!strcmp(dir->d_name , name )) return 1;

        }
        closedir(d);
    }
    return 0;
}

char *GenerateToken(char *str)
{
    size_t size = 33;
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK1234567890!@#$^&*()_+=-";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    FILE * fp;
    fp = fopen(".\\Res\\Tokens.json" , "r");
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    cJSON * tokenList = cJSON_Parse(fileContent);
    for (int i = 0; i < cJSON_GetArraySize(tokenList); ++i) {
        if(!(strcmp(str , cJSON_GetStringValue(cJSON_GetArrayItem(tokenList , i))))) {
            return "";
        }
    }
    cJSON_AddStringToObject(tokenList , currentOrder.information.field1 , str);
    fp = fopen(".\\Res\\Tokens.json" , "w");
    fprintf(fp, "%s" , print = cJSON_Print(tokenList));
    cJSON_free(print);
    fclose(fp);
    free(fileContent);
    return str;
}

char * ConvertTokenToUsername(char * token)
{
    FILE * fp;
    fp = fopen(".\\Res\\Tokens.json" , "r");
    char * fileContent;
    fileContent = (char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    cJSON * tokenList = cJSON_Parse(fileContent);
    for (int i = 0; i < cJSON_GetArraySize(tokenList); ++i) {
        if(!(strcmp(token , cJSON_GetStringValue(cJSON_GetArrayItem(tokenList , i))))) {
           // printf("token belongs to %s\n" , cJSON_GetArrayItem(tokenList , i)->string);
            free(fileContent);
            return cJSON_GetArrayItem(tokenList , i)->string;
        }
    }
}
int IsUserOnline(char * username)
{

    FILE * fp;
    fp = fopen(".\\Res\\Tokens.json" , "r");
    char * fileContent;
    fileContent =(char *)malloc(FILE_CONTENT_SIZE);
    memset(fileContent , 0 , FILE_CONTENT_SIZE);
    fscanf(fp , "%[^\0]s" , fileContent);
    fclose(fp);
    cJSON * tokenList = cJSON_Parse(fileContent);
    for (int i = 0; i < cJSON_GetArraySize(tokenList); ++i) {
        if(!(strcmp(username , cJSON_GetArrayItem(tokenList , i)->string))) {
            free(fileContent);
            return 1;
        }
    }
    free(fileContent);
    return 0;


}
void swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Function to perform Selection Sort
void selectionSort(int * arr, int n)
{
    int i, j, min_idx;

    // One by one move boundary of unsorted subarray
    for (i = 0; i < n - 1; i++) {

        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;

        // Swap the found minimum element
        // with the first element
        swap(&arr[min_idx], &arr[i]);
    }
}