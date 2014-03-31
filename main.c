#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define IDB_no0	0
#define IDB_no1	1
#define IDB_no2	2
#define IDB_no3	3
#define IDB_no4	4
#define IDB_no5	5
#define IDB_no6	6
#define IDB_no7	7
#define IDB_no8	8
#define IDB_no9	9
#define IDB_space 10
#define IDB_background	11

#define IDM_MENU1 1
#define CM_RESTART 101
#define CM_LOAD 102
#define CM_SAVE 103
#define CM_EXIT 104
#define IDI_ICON1 1
#define CM_2x2 1012
#define CM_3x3 1013
#define CM_4x4 1014
#define CM_5x5 1015
#define CM_6x6 1016
#define CM_7x7 1017
#define CM_8x8 1018
#define CM_9x9 1019
#define CM_LOAD 102
#define CM_SAVE 103
#define CM_EXIT 104
#define CM_HINT 201
#define CM_UNDO 202
#define CM_MANUAL 401
#define CM_ABOUT 402
#define CM_5 301
#define CM_10 302
#define CM_20 303
#define CM_30 304
#define CM_60 305
#define CM_120 306
#define CM_240 307
#define CM_INFINITY 308

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
HBITMAP background,no0,no1,no2,no3,no4,no5,no6,no7,no8,no9,space;
HACCEL hAccel;
/*  Make the class name into a global variable  */
char szClassName[ ] = "N-Puzzle";

static TCHAR PlayerChoice='a';
int TimeAllow=0,TimeElapsed=0,TotalTime=0;
int TimeBoxX=400,TimeBoxY=20;
int HintBoxX=400,HintBoxY=120;
int OptionBoxX=400,OptionBoxY=220;
int ChosenMenuItemID = 308;
int error;
/*                      */
/*  Function Prototype  */
/*                      */
void CREATE_STACK(int[]);
int STACK_EMPTY (int[]);
int STACK_FULL (int[]);
void PUSH(int[],int);
int POP(int[]);
void move(int);
int onboard(int,int);
int check_win(void);
void initialize(void);
void reset_end_state(void);
int heuristic(void);
void display(int);
int IDAstar(int,int);
void solve(int[],int);
int load(int[],int[]);
void save(int[],int[]);
void create_puzzle(int[]);

/* Define the maximum size of the puzzle is 9 x 9 */
#define max_size 9
/* Define the maximum number of steps of the stack*/
#define max_length 1201
/*symbol movement*/
const char operation[4][6] = {"Up","Left","Down","Right"};  
/*ordinates of movement in array*/
const int dx[4] = {1,0,-1,0}, dy[4] = {0,1,0,-1};
int solution[50];
/*reverse of corresponding movement*/
const int reverse_dir[4] = {2,3,0,1}; 
int puzzle[max_size][max_size];
int ans;
int cost_limit;
/*ordinates of numbers in puzzle now*/
int nx[max_size*max_size],ny[max_size*max_size];
/*ordinates of numbers in puzzle for end state*/
int fx[max_size*max_size],fy[max_size*max_size];
int size;
int hint;

HINSTANCE hPublicInstance;
HWND hwnd;  
int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)
{
                 /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    
    hPublicInstance=hThisInstance;
    
    hAccel = LoadAccelerators(hThisInstance,"IDA_ACCEL1");
    
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (hThisInstance, MAKEINTRESOURCE(IDI_ICON1));
    wincl.hIconSm = LoadIcon (hThisInstance, MAKEINTRESOURCE(IDI_ICON1));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(IDM_MENU1);                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "N-Puzzle",       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           620,                 /* The programs width */
           480,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        if (!TranslateAccelerator(hwnd,hAccel,&messages))
        {
           /* Translate virtual-key messages into character messages */
           TranslateMessage(&messages);
           /* Send message to WindowProcedure */
           DispatchMessage(&messages);
           }
           }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}



TCHAR filename[100];
int gethint=0;
int s[max_length];    /* Stack storing Stack storing the feasible moves */
int undo[max_length]; /* Stack storing the reverse moves of player      */
int timer;
/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    void print_no(int no,int posx,int posy)
    {
         BITMAP bitmap;
         HDC hdc,bitdc;
         bitdc = CreateCompatibleDC(hdc);
         switch (no)
         {
            case -1:
                 GetObject(space,sizeof(BITMAP),&bitmap);
                 SelectObject(bitdc,space);
                 BitBlt(hdc,posx,posy,bitmap.bmWidth,bitmap.bmHeight,bitdc,0,0,SRCCOPY);
                 break;
            case 0:
                 GetObject(no0,sizeof(BITMAP),&bitmap);
                 SelectObject(bitdc,no0);
                 BitBlt(hdc,posx,posy,bitmap.bmWidth,bitmap.bmHeight,bitdc,0,0,SRCCOPY);
                 break;
            case 1:
                 GetObject(no1,sizeof(BITMAP),&bitmap);
                 SelectObject(bitdc,no1);
                 BitBlt(hdc,posx,posy,bitmap.bmWidth,bitmap.bmHeight,bitdc,0,0,SRCCOPY);
                 break;
            case 2:
                 GetObject(no2,sizeof(BITMAP),&bitmap);
                 SelectObject(bitdc,no2);
                 BitBlt(hdc,posx,posy,bitmap.bmWidth,bitmap.bmHeight,bitdc,0,0,SRCCOPY);
                 break;
            case 3:
                 GetObject(no3,sizeof(BITMAP),&bitmap);
                 SelectObject(bitdc,no3);
                 BitBlt(hdc,posx,posy,bitmap.bmWidth,bitmap.bmHeight,bitdc,0,0,SRCCOPY);
                 break;
            case 4:
                 GetObject(no4,sizeof(BITMAP),&bitmap);
                 SelectObject(bitdc,no4);
                 BitBlt(hdc,posx,posy,bitmap.bmWidth,bitmap.bmHeight,bitdc,0,0,SRCCOPY);
                 break;
            case 5:
                 GetObject(no5,sizeof(BITMAP),&bitmap);
                 SelectObject(bitdc,no5);
                 BitBlt(hdc,posx,posy,bitmap.bmWidth,bitmap.bmHeight,bitdc,0,0,SRCCOPY);
                 break;
            case 6:
                 GetObject(no6,sizeof(BITMAP),&bitmap);
                 SelectObject(bitdc,no6);
                 BitBlt(hdc,posx,posy,bitmap.bmWidth,bitmap.bmHeight,bitdc,0,0,SRCCOPY);
                 break;
            case 7:
                 GetObject(no7,sizeof(BITMAP),&bitmap);
                 SelectObject(bitdc,no7);
                 BitBlt(hdc,posx,posy,bitmap.bmWidth,bitmap.bmHeight,bitdc,0,0,SRCCOPY);
                 break;
            case 8:
                 GetObject(no8,sizeof(BITMAP),&bitmap);
                 SelectObject(bitdc,no8);
                 BitBlt(hdc,posx,posy,bitmap.bmWidth,bitmap.bmHeight,bitdc,0,0,SRCCOPY);
                 break;
            case 9:
                 GetObject(no9,sizeof(BITMAP),&bitmap);
                 SelectObject(bitdc,no9);
                 BitBlt(hdc,posx,posy,bitmap.bmWidth,bitmap.bmHeight,bitdc,0,0,SRCCOPY);
                 break;
            default:                      /* for messages that we don't deal with */
                 break;
            }
         DeleteDC(bitdc);
         }

    HDC hdc,bitdc;
    PAINTSTRUCT ps;
    
    BITMAP bitmap;
    int msgboxID;
    
    DWORD dwWrites;
    char tmp[60];
    static HMENU hMenu;
    RECT TimeBox = {TimeBox.left = TimeBoxX,TimeBox.top = TimeBoxY, 
         TimeBox.right = TimeBoxX+200, TimeBox.bottom = TimeBoxY+80};
    RECT HintBox = {HintBox.left = HintBoxX,HintBox.top = HintBoxY, 
         HintBox.right = HintBoxX+200, HintBox.bottom = HintBoxY+80};
    RECT OptionBox = {OptionBox.left = OptionBoxX,OptionBox.top = OptionBoxY, 
         OptionBox.right = OptionBoxX+200, OptionBox.bottom = OptionBoxY+160};
    int i,j;
    
    int mov=-1;                /* Number represent the move                      */
    int temp;             /* Temporary storge                               */
    
    int temp1,temp10;
    int posx,posy;
    int len;   
    int x,y;
    char buffer[50];
    
    reset_end_state();   
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            background = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_background);
            no0 = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_no0);
            no1 = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_no1);
            no2 = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_no2);
            no3 = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_no3);
            no4 = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_no4);
            no5 = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_no5);
            no6 = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_no6);
            no7 = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_no7);
            no8 = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_no8);
            no9 = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_no9);
            space = LoadBitmap(hPublicInstance,(LPCTSTR) IDB_space);
            hMenu = GetMenu(hwnd);  
            TimeElapsed=0;
            TotalTime=0;
            InvalidateRect(hwnd,NULL,TRUE);
            break;
        case WM_DESTROY:
            KillTimer(hwnd,1);
            PostQuitMessage (0);
            break;
        case WM_COMMAND:
           switch(LOWORD(wParam))
           {
            case CM_SAVE:
                 if (size!=0 && !check_win())
                 {     
                       save(s,undo);
                       InvalidateRect(hwnd,NULL,TRUE);
                       }
                 break;
            case CM_LOAD:
                 temp=load(s,undo);
                 CheckMenuItem(hMenu,ChosenMenuItemID,MF_UNCHECKED); /* uncheck the chosen time limit */
                 ChosenMenuItemID=temp; /* set the chosen time limit to the new time limit */
                 CheckMenuItem(hMenu,ChosenMenuItemID,MF_CHECKED);	/* check the new time limit */
                 switch (ChosenMenuItemID)
                 {
                    case 301:
                         TimeAllow=5;
                         break;
                    case 302:
                         TimeAllow=10;
                         break;
                    case 303:
                         TimeAllow=20;
                         break;
                    case 304:
                         TimeAllow=30;
                         break;
                    case 305:
                         TimeAllow=60;
                         break;
                    case 306:
                         TimeAllow=120;
                         break;
                    case 307:
                         TimeAllow=240;
                         break;
                    case 308:
                         TimeAllow=0;
                         break;
                         }
                 SetTimer(hwnd,1,1000,NULL);
                 InvalidateRect(hwnd,NULL,TRUE);
                 break;
            case CM_EXIT:
                 msgboxID = MessageBox(
                            NULL,
                            "Do you want to exit?",
                            "Confirm Exit",
                            MB_ICONEXCLAMATION | MB_YESNO
                            );
                 if (msgboxID == IDYES)
                    PostQuitMessage (0);
                 break;
            case CM_HINT:
                 if (heuristic()!=0)
                 {          
                    for (i=1;i<=35;i=i+2)
                    solve(s,i);
                    hint=POP(s);
                    PUSH(s,hint);
                    gethint=1;
                    }
              break;
            case CM_UNDO:   
                if ((heuristic()!=0) && undo[0])
                {
                   mov=POP(undo); /* 'Pop' the top move from the undo stack     */
                   move(mov);     /* Move the 'Space' to this direction         */
                   j=POP(s);      /* 'Pop' the top move from the solution stack */
                   if(mov!=j)
                   {
                 /*  This move a different step from the solution           */
                 /*  'Push' the poped step to the top of the solution stack */
                 /*  'Push' this step to the top of the solution stack      */
                     PUSH(s,j);                   
                     PUSH(s,reverse_dir[mov]);
                     }
                     }
                InvalidateRect(hwnd,NULL,TRUE);
                break;
            case CM_MANUAL:
                msgboxID = MessageBox(
                           NULL,
                           "1 2 3        1 2 3\n"
                           "4 5 6   ->   4 5 6\n"
                           "7   8        7 8\n"
                           "You need to move the\n"
                           "puzzle until all boxes in\n"
                           "an ascending order and the\n"
                           "space at the right bottom.\n"
                           "\nHot Keys:\n"
                           "Help               F1\n"
                           "Save           Ctrl+S\n"
                           "Load           Ctrl+L\n"
                           "Exit           Alt+F4\n"
                           "Hint           Ctrl+H\n"
                           "Undo           Ctrl+U\n"
                           "N*N Puzzle      F2~F9\n",
                           "User Manual",
                           MB_OK
                           );
                 break;
            case CM_ABOUT:
                msgboxID = MessageBox(
                           NULL,
                           "Powered By TWK",
                           "N-Puzzle Game",
                           MB_OK
                           );
                 break;             
            case CM_2x2:
                size=2;
                gethint=0;
                CREATE_STACK(undo);
                create_puzzle(s);
                SetTimer(hwnd,1,1000,NULL);
                timer=1;
                SendMessage(hwnd,WM_CREATE,0,0);
                break;
            case CM_3x3:
                size=3;
                gethint=0;
                CREATE_STACK(undo);
                create_puzzle(s);
                SetTimer(hwnd,1,1000,NULL);
                timer=1;
                SendMessage(hwnd,WM_CREATE,0,0);
                break;
            case CM_4x4:
                size=4;
                gethint=0;
                CREATE_STACK(undo);
                create_puzzle(s);
                SetTimer(hwnd,1,1000,NULL);
                timer=1;
                SendMessage(hwnd,WM_CREATE,0,0);
                break;
            case CM_5x5:
                size=5;
                gethint=0;
                CREATE_STACK(undo);
                create_puzzle(s);
                SetTimer(hwnd,1,1000,NULL);
                timer=1;
                SendMessage(hwnd,WM_CREATE,0,0);
                break;
            case CM_6x6:
                size=6;
                gethint=0;
                CREATE_STACK(undo);
                create_puzzle(s);
                SetTimer(hwnd,1,1000,NULL);
                timer=1;
                SendMessage(hwnd,WM_CREATE,0,0);
                break;
            case CM_7x7:
                size=7;
                gethint=0;
                CREATE_STACK(undo);
                create_puzzle(s);
                SetTimer(hwnd,1,1000,NULL);
                timer=1;
                SendMessage(hwnd,WM_CREATE,0,0);
                break;
            case CM_8x8:
                size=8;
                gethint=0;
                CREATE_STACK(undo);
                create_puzzle(s);
                SetTimer(hwnd,1,1000,NULL);
                timer=1;
                SendMessage(hwnd,WM_CREATE,0,0);
                break;
            case CM_9x9:
                size=9;
                gethint=0;
                CREATE_STACK(undo);
                create_puzzle(s);
                SetTimer(hwnd,1,1000,NULL);
                timer=1;
                SendMessage(hwnd,WM_CREATE,0,0);
                break;
            case CM_5:
                TimeAllow = 5;	/* set the time limit to 5 seconds */
                CheckMenuItem(hMenu,LOWORD(wParam),MF_CHECKED);	/* check the new time limit */
                CheckMenuItem(hMenu,ChosenMenuItemID,MF_UNCHECKED); /* uncheck the chosen time limit */
                ChosenMenuItemID = LOWORD(wParam); /* set the chosen time limit to 5 seconds */
                break;
            case CM_10:
                TimeAllow = 10;	
                CheckMenuItem(hMenu,LOWORD(wParam),MF_CHECKED);	
                CheckMenuItem(hMenu,ChosenMenuItemID,MF_UNCHECKED); 
                ChosenMenuItemID = LOWORD(wParam); 
                break;
            case CM_20:
                TimeAllow = 20;	
                CheckMenuItem(hMenu,LOWORD(wParam),MF_CHECKED);	
                CheckMenuItem(hMenu,ChosenMenuItemID,MF_UNCHECKED); 
                ChosenMenuItemID = LOWORD(wParam); 
                break;
            case CM_30:
                TimeAllow = 30;	
                CheckMenuItem(hMenu,LOWORD(wParam),MF_CHECKED);	
                CheckMenuItem(hMenu,ChosenMenuItemID,MF_UNCHECKED); 
                ChosenMenuItemID = LOWORD(wParam); 
                break;
            case CM_60:
                TimeAllow = 60;	
                CheckMenuItem(hMenu,LOWORD(wParam),MF_CHECKED);	
                CheckMenuItem(hMenu,ChosenMenuItemID,MF_UNCHECKED); 
                ChosenMenuItemID = LOWORD(wParam); 
                break;
            case CM_120:
                TimeAllow = 120;
                CheckMenuItem(hMenu,LOWORD(wParam),MF_CHECKED);	
                CheckMenuItem(hMenu,ChosenMenuItemID,MF_UNCHECKED);
                ChosenMenuItemID = LOWORD(wParam); 
                break;
            case CM_240:
                TimeAllow = 240;
                CheckMenuItem(hMenu,LOWORD(wParam),MF_CHECKED);	
                CheckMenuItem(hMenu,ChosenMenuItemID,MF_UNCHECKED);
                ChosenMenuItemID = LOWORD(wParam); 
                break;
            case CM_INFINITY:
                TimeAllow = 0;
                CheckMenuItem(hMenu,LOWORD(wParam),MF_CHECKED);
                CheckMenuItem(hMenu,ChosenMenuItemID,MF_UNCHECKED);
                ChosenMenuItemID = LOWORD(wParam); 
                break;
      };
      break;
      case WM_PAINT:
      hdc = BeginPaint(hwnd,&ps);
      /* begin to paint bitmaps */
      if (size)
      {
         bitdc = CreateCompatibleDC(hdc);
         GetObject(background,sizeof(BITMAP),&bitmap);
         SelectObject(bitdc,background);
         BitBlt(hdc,21*(max_size-size)+5,20,size*32+(size+1)*10,size*32+(size+1)*10,bitdc,0,0,SRCCOPY);
         for (i=0;i<size;i++)
         {
            for (j=0;j<size;j++)
            {
              temp10=puzzle[i][j]/10%10;
              temp1=puzzle[i][j]%10;
              if (temp1==0 && temp10==0)
              {
                 temp1=99;
                 temp10=99;
                 }
              else if (temp10==0)
                 temp10=-1;
              posx=21*(max_size-size)+j*42+15;
              posy=i*42+30;
              print_no(temp10,posx,posy);
              posx=21*(max_size-size)+j*42+31;
              posy=i*42+30;
              print_no(temp1,posx,posy);
              }
              }
              }
      DeleteDC(bitdc);
      /* end the painting */
      SetTextColor(hdc,0x000000ff);
      Rectangle(hdc,TimeBox.left,TimeBox.top,TimeBox.right,TimeBox.bottom);
      TextOut(hdc,TimeBoxX+15,TimeBoxY+25,tmp,wsprintf(tmp,"Total Time : %d s",TotalTime));
      TextOut(hdc,TimeBoxX+15,TimeBoxY+45,tmp,wsprintf(tmp,"Time elapsed : %d s",TimeElapsed));
      Rectangle(hdc,HintBox.left,HintBox.top,HintBox.right,HintBox.bottom);
      SetTextColor(hdc,0x0000ff00);
      TextOut(hdc,HintBoxX+15,HintBoxY+30,tmp,wsprintf(tmp,"Hint : "));
      if (gethint==1)      
         TextOut(hdc,HintBoxX+80,HintBoxY+30,tmp,wsprintf(tmp,"%s",operation[hint]));
      Rectangle(hdc,OptionBox.left,OptionBox.top,OptionBox.right,OptionBox.bottom);
      SetTextColor(hdc,0x00ff0000);
      TextOut(hdc,OptionBoxX+15,OptionBoxY+25,tmp,wsprintf(tmp,"New Game : F1-F9 ( N * N )"));
      TextOut(hdc,OptionBoxX+15,OptionBoxY+50,tmp,wsprintf(tmp,"Move : Arrow Keys"));
      TextOut(hdc,OptionBoxX+60,OptionBoxY+75,tmp,wsprintf(tmp,"or Left Click"));
      TextOut(hdc,OptionBoxX+15,OptionBoxY+100,tmp,wsprintf(tmp,"Hint : Crrl+H"));
      TextOut(hdc,OptionBoxX+15,OptionBoxY+125,tmp,wsprintf(tmp,"Undo : Crrl+U"));
      EndPaint(hwnd,&ps);
      break;
      case WM_KEYDOWN:
           switch (wParam)
           {
               case VK_UP:
                 gethint=0;
                 if (check_win()!=1)
                 {
                    TimeElapsed=0;            
                    mov=0;
                    if (onboard(nx[0]+dx[mov],ny[0]+dy[mov]))
                    {   
                       /* Move the 'Space' */
                       move(mov);
                       /* 'Pop' the top move from the solution stack */
                       j=POP(s);
                       if(mov!=j)
                       {
                             /*  The player move a different step from the solution     */
                             /*  'Push' the poped step to the top of the solution stack */
                             /*  'Push' this step to the top of the solution stack      */
                             PUSH(s,j);                   
                             PUSH(s,reverse_dir[mov]);
                             }
                       /*  'Push' this step to the top of the undo stack      */
                       PUSH(undo,reverse_dir[mov]);
                       }
                       InvalidateRect(hwnd,NULL,TRUE);
                       if (check_win()==1)
                       {
                             msgboxID = MessageBox(
                                        NULL,
                                        "You win!!!",
                                        "Congratulation",
                                        MB_OK
                                        );
                                        }
                                        }
               break;
               case VK_LEFT:
                 gethint=0;
                 if (check_win()!=1)
                 {
                    TimeElapsed=0;            
                    mov=1;
                    if (onboard(nx[0]+dx[mov],ny[0]+dy[mov]))
                    {   
                       /* Move the 'Space' */
                       move(mov);
                       /* 'Pop' the top move from the solution stack */
                       j=POP(s);
                       if(mov!=j)
                       {
                             /*  The player move a different step from the solution     */
                             /*  'Push' the poped step to the top of the solution stack */
                             /*  'Push' this step to the top of the solution stack      */
                             PUSH(s,j);                   
                             PUSH(s,reverse_dir[mov]);
                             }
                       /*  'Push' this step to the top of the undo stack      */
                       PUSH(undo,reverse_dir[mov]);
                       }
                       InvalidateRect(hwnd,NULL,TRUE);
                       if (check_win()==1)
                       {
                             msgboxID = MessageBox(
                                        NULL,
                                        "You win!!!",
                                        "Congratulation",
                                        MB_OK
                                        );
                                        }
                                        }
               break;
               case VK_DOWN:
                 gethint=0;
                 if (check_win()!=1)
                 {
                    TimeElapsed=0;            
                    mov=2;
                    if (onboard(nx[0]+dx[mov],ny[0]+dy[mov]))
                    {   
                       /* Move the 'Space' */
                       move(mov);
                       /* 'Pop' the top move from the solution stack */
                       j=POP(s);
                       if(mov!=j)
                       {
                             /*  The player move a different step from the solution     */
                             /*  'Push' the poped step to the top of the solution stack */
                             /*  'Push' this step to the top of the solution stack      */
                             PUSH(s,j);                   
                             PUSH(s,reverse_dir[mov]);
                             }
                       /*  'Push' this step to the top of the undo stack      */
                       PUSH(undo,reverse_dir[mov]);
                       }
                       InvalidateRect(hwnd,NULL,TRUE);
                       if (check_win()==1)
                       {
                             msgboxID = MessageBox(
                                        NULL,
                                        "You win!!!",
                                        "Congratulation",
                                        MB_OK
                                        );
                                        }
                                        }
               break;
               case VK_RIGHT:
                 gethint=0;
                 if (check_win()!=1)
                 {
                    TimeElapsed=0;            
                    mov=3;
                    if (onboard(nx[0]+dx[mov],ny[0]+dy[mov]))
                    {   
                       /* Move the 'Space' */
                       move(mov);
                       /* 'Pop' the top move from the solution stack */
                       j=POP(s);
                       if(mov!=j)
                       {
                             /*  The player move a different step from the solution     */
                             /*  'Push' the poped step to the top of the solution stack */
                             /*  'Push' this step to the top of the solution stack      */
                             PUSH(s,j);                   
                             PUSH(s,reverse_dir[mov]);
                             }
                       /*  'Push' this step to the top of the undo stack      */
                       PUSH(undo,reverse_dir[mov]);
                       }
                       InvalidateRect(hwnd,NULL,TRUE);
                       if (check_win()==1)
                       {
                             msgboxID = MessageBox(
                                        NULL,
                                        "You win!!!",
                                        "Congratulation",
                                        MB_OK
                                        );
                                        }
                                        }
               break;
      }
      break;
      case WM_LBUTTONUP:
        gethint=0;
        if (check_win()!=1)
        {
           if (HIWORD(lParam)-25>=0 && LOWORD(lParam)-21*(max_size-size)-10>=0)
           {
              x=(HIWORD(lParam)-25)/42;
              y=(LOWORD(lParam)-21*(max_size-size)-10)/42;
              if (x>=0 && x<size && y>=0 && y<size)
              {
                 if (abs(x-nx[0])+abs(y-ny[0])==1)
                 {
                      for (i=0;i<4;i++)
                      {
                          if (x-nx[0]==dx[i] && y-ny[0]==dy[i])
                          { 
                           TimeElapsed=0;
                           move(i);
                           j=POP(s);
                           if(i!=j)
                           {
                                   /*  The player move a different step from the solution     */
                                   /*  'Push' the poped step to the top of the solution stack */
                                   /*  'Push' this step to the top of the solution stack      */
                                   PUSH(s,j);                   
                                   PUSH(s,reverse_dir[i]);
                                   }
                           /*  'Push' this step to the top of the undo stack      */
                           PUSH(undo,reverse_dir[i]);
                           }
                           }
                           }
                           }
                           }
        InvalidateRect(hwnd,NULL,TRUE);
        if (check_win()==1)
        {
         msgboxID = MessageBox(
                    NULL,
                    "You win!!!",
                    "Congratulation",
                    MB_OK
                    );
                    }
                    }
         break;         
      case WM_TIMER:
        TimeElapsed++;
        TotalTime++;
        if (check_win()==1)
            KillTimer(hwnd,1);
        else
        {
            InvalidateRect(hwnd,&TimeBox,TRUE);
            InvalidateRect(hwnd,&HintBox,TRUE);
            }
        if ((TimeElapsed>TimeAllow)&&(TimeAllow>0))
        {
             KillTimer(hwnd,1);
             msgboxID = MessageBox(
                        NULL,
                        "You lose!!!",
                        "",
                        MB_OK
                        );
                        } 
      break;
      default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
      }
      return 0;
}

void CREATE_STACK(int STACK[max_length])
{
     int i;
     STACK[0]=0;
     for (i=1;i<max_length;i++)
         STACK[i]=-1;
}

/*                                                      */
/*  Check whether the stack is empty                    */
/*                                                      */
/*  Input:  Stack storing the feasible moves            */
/*  Output: integer '0' means the stack is not empty    */
/*          integer '1' means the stack is empty        */
/*                                                      */
int STACK_EMPTY (int STACK[max_length])
{
     if (STACK[0]==0)
        return 1;
     else
        return 0;
}

/*                                                      */
/*  Check whether the stack is full                     */
/*                                                      */
/*  Input:  Stack storing the feasible moves            */
/*  Output: integer '0' means the stack is not full     */
/*          integer '1' means the stack is full         */
/*                                                      */         
int STACK_FULL (int STACK[max_length])
{
     if (STACK[0]==max_length-1)
        return 1;
     else
        return 0;
}

/*                                                      */
/*  Push a number to the top stack                      */
/*                                                      */
/*  Input:  Stack storing the feasible moves            */
/*  Output: Void                                        */
/*                                                      */         
void PUSH(int STACK[max_length],int STACK_ITEM)
{
     if (STACK_FULL(STACK))
       error=0;
     else
     {
        STACK[0]++;
        STACK[STACK[0]]=STACK_ITEM;
        }
}

/*                                                      */
/*  Pop a number from the stack                         */
/*                                                      */
/*  Input:  Stack storing the feasible moves            */
/*  Output: The number on the top of the stack          */
/*                                                      */ 
int POP(int STACK[max_length])
{
     if (STACK_EMPTY(STACK))
       error=1;
     else
     {
        STACK[0]--;
        }
        return STACK[STACK[0]+1];
}

/*                                                         */
/*  Swap the space with the number nearby                  */
/*                                                         */
/*  Input:  Number representing the direction of move  */
/*  Output: Void                                           */
/*                                                         */ 
void move(int dir)
{
     int tempx,tempy;
     /* Swap ordinates */
     tempx=nx[0]; /* Row number of the 'Space' */ 
     tempy=ny[0]; /* Column number of the 'Space' */
     nx[0]=nx[0]+dx[dir]; /* New row number of the 'Space' */
     ny[0]=ny[0]+dy[dir]; /* New column number of the 'Space' */
     nx[puzzle[nx[0]][ny[0]]]=tempx; /* New row number of another tile */
     ny[puzzle[nx[0]][ny[0]]]=tempy; /* New column number of another tile */
     /* Swap the puzzle */
     int temp=puzzle[tempx][tempy];
     puzzle[tempx][tempy]=puzzle[nx[0]][ny[0]];
     puzzle[nx[0]][ny[0]]=temp;
}

/*                                                           */
/*  Check whethter the move is available                 */
/*                                                           */
/*  Input:  New ordinates of position of 'Space'             */
/*  Output: integer '0' means the position is available      */
/*          integer '1' means the position is not available  */
/*                                                           */ 
int onboard(int x,int y)  
{  
     if (x>=0 && x<size&& y>=0 && y<size)
        return 1;
     else
        return 0;  
}  

/*                                                           */
/*  Check whethter the player win                            */
/*                                                           */
/*  Input:  Void                                             */
/*  Output: integer '0' means the player does not win        */
/*          integer '1' means the player wins                */
/*                                                           */
int check_win(void)
{
     int i=1;
     int w=1;
     while (i<size*size && w==1)
     {
        if ((nx[i]!=fx[i]) || (ny[i]!=fy[i]))
            w=0;
        i++;
        }
     return w;
}

/*                                                           */
/*  Initialize the puzzle and ordinates of numbers           */
/*                                                           */
/*  Input:  Void                                             */
/*  Output: Void                                             */
/*                                                           */               
void initialize(void)
{
     int i,j;
     for (i=0;i<max_size;i++)
        for (j=0;j<max_size;j++)
            puzzle[i][j]=-1;
     for (i=0;i<size;i++)
        for (j=0;j<size;j++)
            puzzle[i][j]=i*size+j+1;
     puzzle[size-1][size-1]=0;
     nx[0]=size-1;
     ny[0]=size-1;
     for (i=1;i<size*size;i++)
     {
        j=i-1;
        nx[i]=j/size;
        ny[i]=j%size;
        }
}

/*                                                           */
/*  Rest the ordinates of number at end state                */
/*                                                           */
/*  Input:  Void                                             */
/*  Output: Void                                             */
/*                                                           */ 
void reset_end_state(void)
{
     int i,j;
     fx[0]=size-1;
     fy[0]=size-1;
     for (i=1;i<size*size;i++)
     {
        j=i-1;
        fx[i]=j/size;
        fy[i]=j%size;
        }
}

/*                                                           */
/*  Create a puzzle game                                     */
/*                                                           */
/*  Input:  Stack storing the feasible moves                 */
/*  Output: Void                                             */
/*                                                           */                            
void create_puzzle(int STACK[max_length])
{
     int i=0;
     int dir;
     int pre_dir=-1; /* Last move */
     int n_step;
     initialize();
     CREATE_STACK(STACK);
     /* Set number of moves of the puzzle */
     srand(time(NULL)); /* Create seed of random */
     n_step=(rand()%175+1)+size*25;
     while (i<n_step)
     {
        dir=rand()%4;
        if (reverse_dir[dir]!=pre_dir && onboard(nx[0]+dx[dir],ny[0]+dy[dir]))
        {
           /* The move is the reverse of previous move and available */
           /* 'Push' this step to the top of the solution stack      */
           /* Store this step as previous step                       */
           move(dir);
           PUSH(STACK,reverse_dir[dir]);
           pre_dir=dir;
           i++;
           }
           }
     if (heuristic()==0)               
        create_puzzle(STACK);
}

/*                                                           */
/*  Find the heuristic cost that to reach the end state      */
/*                                                           */
/*  Input:  Void                                             */
/*  Output: The heuristic cost                               */
/*                                                           */           
int heuristic(void)
{
     int i,c=0;
     for (i=1;i<size*size;i++) 
     {
        /* Total of absolute distance of all numbers */  
        c+=abs(fx[i]-nx[i]);           
        c+=abs(fy[i]-ny[i]); 
        }
     return c;
}

/*                                                           */
/*  Find the possible route of moves                         */
/*                                                           */
/*  Input:  Row number of 'Space'                            */
/*          Column number of 'Space'                         */
/*          Number of steps moved                            */
/*          Direction of previous move                       */
/*  Output: cost_limit of route                                   */
/*                                                           */
int IDAstar(int g,int prev_dir)  
{ 
     int h=heuristic(); /* The heuristic cost */
     int i;  
     if (g+h>cost_limit)
     /* The total cost is larger than the cost_limit */
     /* Return the total cost                   */
         return g+h;  
     if (h==0) 
     {
        /* The heuristic cost is '0' */
        /* The answer found          */
        /* Return number of steps    */
        ans=1;
        return g;
        }  
     int next_cost_limit=1e9;  /* Initialize the new cost_limit */
     for (i=0;i<4;i++)                       
     {
        /* The direct of move is the same as previous one */
        /* Jump the move                                  */
        if (reverse_dir[i]==prev_dir)
           continue;  
        /* The new ordinates of 'Space' is not on the board */
        /* Jump the move                                    */
        if (!onboard(nx[0]+dx[i],ny[0]+dy[i]))
           continue;           
        solution[g]=i;    /* Store the move */               
        move(i);         /* Move the 'Space' */ 
        int new_cost_limit=IDAstar(g+1,i);  
        if (ans)
           return new_cost_limit;
        /* Get the minimum of cost_limit */ 
        next_cost_limit=min(next_cost_limit,new_cost_limit); 
        /* Restore to the previous state */  
        move(reverse_dir[i]); 
        }  
     return next_cost_limit;  
}

/*                                                           */
/*  Find the solution                                        */
/*                                                           */
/*  Input:  Stack storing the feasible moves                 */
/*          cost_limit of cost                                    */
/*  Output: Void                                             */
/*                                                           */
void solve(int STACK[max_length],int n)
{
     int i,j,k;
     int count=0; /* Count for the number of step */
     int STACK2[max_length];  /* Stack to store temporary moves */
     int temp[max_size][max_size]; /* Store temporary puzzle */
     int x[max_size*max_size],y[max_size*max_size]; /* Store temporary state */
     int tx[max_size*max_size],ty[max_size*max_size]; /* Store temporary state */
     ans = 0;  
     CREATE_STACK(STACK2); 
     for (i=0;i<size;i++)
         for (j=0;j<size;j++)
         {
             /* Save the initial state of puzzle and ordinates */
             temp[i][j]=puzzle[i][j];
             x[i*size+j]=nx[i*size+j];
             y[i*size+j]=ny[i*size+j];
             }
     while (heuristic()!=0)
     {
           ans=0;
           while (!ans)
           {
                  /* Save the state */
                  for (i=0;i<size*size;i++)
                  {
                      tx[i]=nx[i];
                      ty[i]=ny[i];
                      }
                  cost_limit = 0;
                  /* Find solution within 'n' steps */
                  while (!ans && cost_limit<= n)  
                        cost_limit=IDAstar(0,-1);
                  if (ans)
                  {
                        /* Get the number of steps should be 'Pop' from the solution stack */
                        j=STACK[0]-count;
                        /* 'Pop' 'j' moves */
                        for (i=0;i<j;i++)
                            k=POP(STACK);
                        /* 'Push' the moves of solution to the solution stack */    
                        for (i=0;i<cost_limit;i++)
                            PUSH(STACK,solution[cost_limit-i-1]);
                        /* 'Push' the moves stored in temporary stack back to the solution stack */ 
                        j=STACK2[0];
                        for (i=0;i<j;i++)
                            PUSH(STACK,POP(STACK2));
                        /* Set end state to the state stored before */    
                        for (i=0;i<size*size;i++)
                        {
                            fx[i]=tx[i];
                            fy[i]=ty[i];
                        }
                        count+=cost_limit; /* Update the count of steps */
                        /* Restore the puzzle and ordinates to the initial state */
                        for (i=0;i<size;i++)
                            for (j=0;j<size;j++)
                            {
                                puzzle[i][j]=temp[i][j];
                                nx[i*size+j]=x[i*size+j];
                                ny[i*size+j]=y[i*size+j];
                                }
                  }
                  else
                  {
                        /* 'Pop' a move from the solution stack     */
                        /* Move the puzzle to that direction        */
                        /* 'Push the move intio the temporary stack */
                        k=POP(STACK);
                        move(k);
                        PUSH(STACK2,k);
                        }
                        }  
                        }
     /* Reset end state */       
     reset_end_state();
}

/*                                                           */
/*  Load the saved game                                      */
/*                                                           */
/*  Input:  Stack storing the feasible moves                 */
/*          Stack storing the reverse moves of player        */
/*  Output: Void                                             */
/*                                                           */                                        
int load(int STACK[max_length],int UNDO_STACK[max_length])
{
     HANDLE hFile;
     int i,j,len,mov,msgboxID,digit;
     int ok=0;
     DWORD dwReads;
     int MenuItemID;
     char temp[2];
     char buffer[100];
     OPENFILENAME ofn;
     char szFileName[MAX_PATH] = "";
     ZeroMemory(&ofn, sizeof(ofn));
     ofn.lStructSize = sizeof(ofn); 
     ofn.hwndOwner = hwnd;
     ofn.lpstrFilter = "Save Files (*.sav)\0*.sav\0";
     ofn.lpstrFile = szFileName;
     ofn.nMaxFile = MAX_PATH;
     ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
     ofn.lpstrDefExt = "sav";
     if(GetOpenFileName(&ofn))
     {
           CREATE_STACK(STACK);
           CREATE_STACK(UNDO_STACK);
           ZeroMemory(buffer,sizeof(buffer)); 
           hFile=CreateFile(szFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
           ReadFile(hFile,buffer,strlen("N-Puzzle by TWK"),&dwReads,NULL);
           if (strcmp(buffer,"N-Puzzle by TWK")!=0)
           {
                 msgboxID = MessageBox(
                            NULL,
                            "This is not the save of this game!",
                            "",
                            MB_ICONEXCLAMATION | MB_OK
                            );
                            }
           else{
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("SIZE OF PUZZLE"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 ReadFile(hFile,buffer,1,&dwReads,NULL);
                 size=atoi(buffer);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("GAME BOARD"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 for (i=0;i<size;i++)
                     for(j=0;j<size;j++)                           
                     {
                            ReadFile(hFile,buffer,1,&dwReads,NULL);
                            puzzle[i][j]=atoi(buffer)*10;
                            ReadFile(hFile,buffer,1,&dwReads,NULL);
                            puzzle[i][j]+=atoi(buffer);
                            ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                            }
                 ReadFile(hFile,buffer,strlen("X-COORDINATES"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 for (i=0;i<size*size;i++)
                 {
                     ReadFile(hFile,temp,1,&dwReads,NULL);
                     nx[i]=atoi(temp);
                     ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                     }
                 ReadFile(hFile,buffer,strlen("Y-COORDINATES"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 for (i=0;i<size*size;i++)
                 {
                     ReadFile(hFile,temp,1,&dwReads,NULL);
                     ny[i]=atoi(temp);
                     ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                     }
                 /* Get number of moves in solution stack */
                 ReadFile(hFile,buffer,strlen("DIGITS OF STACK SIZE"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 ZeroMemory(buffer,sizeof(buffer));                 
                 ReadFile(hFile,temp,1,&dwReads,NULL);
                 digit=atoi(temp);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("SIZE OF SOLUTION STACK"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 ReadFile(hFile,buffer,digit,&dwReads,NULL);
                 len=atoi(buffer);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("MOVES STORED IN STACK"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 /* Get the moves of undo stack */
                 for (i=0;i<len;i++)
                 {
                     ReadFile(hFile,temp,1,&dwReads,NULL);
                     mov=atoi(temp);
                     PUSH(STACK,mov);
                     ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                     }
                 /* Get number of moves in undo stack */
                 ReadFile(hFile,buffer,strlen("DIGITS OF STACK SIZE"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 ZeroMemory(buffer,sizeof(buffer));                 
                 ReadFile(hFile,temp,1,&dwReads,NULL);
                 digit=atoi(temp);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 if (digit)
                 {
                    ReadFile(hFile,buffer,strlen("SIZE OF UNDO STACK"),&dwReads,NULL);
                    ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                    ReadFile(hFile,buffer,digit,&dwReads,NULL);
                    len=atoi(buffer);
                    ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                    /* Get the moves of undo stack */
                    ReadFile(hFile,buffer,strlen("MOVES STORED IN STACK"),&dwReads,NULL);
                    ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                    for (i=0;i<len;i++)
                    {
                        ReadFile(hFile,temp,1,&dwReads,NULL);
                        mov=atoi(temp);
                        PUSH(UNDO_STACK,mov);
                        ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                        }
                        }
                 ReadFile(hFile,buffer,strlen("TIME LIMIT ID"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 ZeroMemory(buffer,sizeof(buffer));          
                 ReadFile(hFile,buffer,3,&dwReads,NULL);
                 MenuItemID=atoi(buffer);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 ZeroMemory(buffer,sizeof(buffer));
                 ReadFile(hFile,buffer,strlen("DIGITS OF TOTAL TIME"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);           
                 ReadFile(hFile,temp,1,&dwReads,NULL);
                 digit=atoi(temp);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 if (digit)
                 {
                    ReadFile(hFile,buffer,strlen("TOTAL TIME"),&dwReads,NULL);
                    ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);         
                    ReadFile(hFile,buffer,digit,&dwReads,NULL);
                    TotalTime=atoi(buffer);
                    ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                    }
                 ReadFile(hFile,buffer,strlen("DIGITS OF TIME ELAPSED IN CURRENT TURN"),&dwReads,NULL);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 ZeroMemory(buffer,sizeof(buffer));           
                 ReadFile(hFile,temp,1,&dwReads,NULL);
                 digit=atoi(temp);
                 ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                 if (digit)
                 {
                    ReadFile(hFile,buffer,strlen("TIME ELAPSED IN CURRENT TURN"),&dwReads,NULL);
                    ReadFile(hFile,buffer,strlen("\r\n"),&dwReads,NULL);
                    ReadFile(hFile,buffer,digit,&dwReads,NULL);
                    TimeElapsed=atoi(buffer);
                    }
                    }
                    }
     else
         return ChosenMenuItemID;
     CloseHandle(hFile);
     return MenuItemID;
}

/*                                                           */
/*  Save the game                                            */
/*                                                           */
/*  Input:  Stack storing the feasible moves                 */
/*          Stack storing the reverse moves of player        */
/*  Output: Void                                             */
/*                                                           */        
void save(int STACK[max_length],int UNDO_STACK[max_length])
{
     HANDLE hFile;
     int i,j,len,mov,msgboxID,digit;
     int s[max_length];
     CREATE_STACK(s);
     DWORD dwWrites;
     char buffer[100];
     int temp;
     int ok=1;
     OPENFILENAME ofn;
     char szFileName[MAX_PATH] = "";
     ZeroMemory(&ofn, sizeof(ofn));
     ofn.lStructSize = sizeof(ofn); 
     ofn.hwndOwner = hwnd;
     ofn.lpstrFilter = "Save Files (*.sav)\0*.sav\0";
     ofn.lpstrFile = szFileName;
     ofn.nMaxFile = MAX_PATH;
     ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
     ofn.lpstrDefExt = "sav";
     if(GetSaveFileName(&ofn))
     {
          hFile=CreateFile(szFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
          WriteFile(hFile,"N-Puzzle by TWK",strlen("N-Puzzle by TWK"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          WriteFile(hFile,"SIZE OF PUZZLE",strlen("SIZE OF PUZZLE"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          itoa(size,buffer,10);
          WriteFile(hFile,buffer,1,&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          WriteFile(hFile,"GAME BOARD",strlen("GAME BOARD"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);  
          /* Write the order of puzzle */
          for (i=0;i<size;i++)
              for (j=0;j<size;j++)
              {              
                   itoa(puzzle[i][j],buffer,10);
                   if (strlen(buffer)<2)
                      WriteFile(hFile,"0",1,&dwWrites,NULL);
                   WriteFile(hFile,buffer,strlen(buffer),&dwWrites,NULL);
                   WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
                   }
          /* Write the ordinates of puzzle */
          WriteFile(hFile,"X-COORDINATES",strlen("X-COORDINATES"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          for (i=0;i<size*size;i++)
          {              
              itoa(nx[i],buffer,10);
              WriteFile(hFile,buffer,1,&dwWrites,NULL);
              WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
              }
          WriteFile(hFile,"Y-COORDINATES",strlen("Y-COORDINATES"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          for (i=0;i<size*size;i++)
          {              
              itoa(ny[i],buffer,10);
              WriteFile(hFile,buffer,1,&dwWrites,NULL);
              WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
              }
          /* Write the number of moves in solution stack */
          WriteFile(hFile,"DIGITS OF STACK SIZE",strlen("DIGITS OF STACK SIZE"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          len=STACK[0];
          temp=len;
          digit=0;
          while(temp>0)
          {
              digit++;
              temp=temp/10;
              } 
          itoa(digit,buffer,10);
          WriteFile(hFile,buffer,strlen(buffer),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          WriteFile(hFile,"SIZE OF SOLUTION STACK",strlen("SIZE OF SOLUTION STACK"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);                       
          itoa(len,buffer,10);
          WriteFile(hFile,buffer,strlen(buffer),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          /* Write the moves of soltion stack */
          WriteFile(hFile,"MOVES STORED IN STACK",strlen("MOVES STORED IN STACK"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          for (i=0;i<len;i++)
              PUSH(s,POP(STACK));
          for (i=0;i<len;i++)
          {
              temp=POP(s);
              itoa(temp,buffer,10);
              WriteFile(hFile,buffer,1,&dwWrites,NULL);
              WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
              PUSH(STACK,temp);
              }
          /* Write the number of moves in undo stack */
          WriteFile(hFile,"DIGITS OF STACK SIZE",strlen("DIGITS OF STACK SIZE"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          len=UNDO_STACK[0];
          temp=len;
          digit=0;
          while(temp>0)
          {
              digit++;
              temp=temp/10;
              } 
          itoa(digit,buffer,10);
          WriteFile(hFile,buffer,strlen(buffer),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          if (digit)
          {     
              WriteFile(hFile,"SIZE OF UNDO STACK",strlen("SIZE OF UNDO STACK"),&dwWrites,NULL);
              WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
              itoa(len,buffer,10);
              WriteFile(hFile,buffer,strlen(buffer),&dwWrites,NULL);
              WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
              /* Write the moves in undo stack */
              WriteFile(hFile,"MOVES STORED IN STACK",strlen("MOVES STORED IN STACK"),&dwWrites,NULL);
              WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
              for (i=0;i<len;i++)
                  PUSH(s,POP(UNDO_STACK));
              for (i=0;i<len;i++)
              {
                  temp=POP(s);
                  itoa(temp,buffer,10);
                  WriteFile(hFile,buffer,1,&dwWrites,NULL);
                  WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
                  PUSH(UNDO_STACK,temp);
                  }
                  }
          WriteFile(hFile,"TIME LIMIT ID",strlen("TIME LIMIT ID"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          itoa(ChosenMenuItemID,buffer,10);
          WriteFile(hFile,buffer,strlen(buffer),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          WriteFile(hFile,"DIGITS OF TOTAL TIME",strlen("DIGITS OF TOTAL TIME"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL); 
          temp=TotalTime;
          digit=0;
          while(temp>0)
          {
              digit++;
              temp=temp/10;
              } 
          itoa(digit,buffer,10);
          WriteFile(hFile,buffer,strlen(buffer),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          if (digit)
          {      
              WriteFile(hFile,"TOTAL TIME",strlen("TOTAL TIME"),&dwWrites,NULL);
              WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
              itoa(TotalTime,buffer,10);
              WriteFile(hFile,buffer,strlen(buffer),&dwWrites,NULL);
              WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL); 
              }
          WriteFile(hFile,"DIGITS OF TIME ELAPSED IN CURRENT TURN",strlen("DIGITS OF TIME ELAPSED IN CURRENT TURN"),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          temp=TimeElapsed;
          digit=0;
          while(temp>0)
          {
              digit++;
              temp=temp/10;
              } 
          itoa(digit,buffer,10);
          WriteFile(hFile,buffer,strlen(buffer),&dwWrites,NULL);
          WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
          if (digit)
          { 
              WriteFile(hFile,"TIME ELAPSED IN CURRENT TURN",strlen("TIME ELAPSED IN CURRENT TURN"),&dwWrites,NULL);
              WriteFile(hFile,"\r\n",strlen("\r\n"),&dwWrites,NULL);
              itoa(TimeElapsed,buffer,10);
              WriteFile(hFile,buffer,strlen(buffer),&dwWrites,NULL);
              }                       
              }                        
     CloseHandle(hFile);
}
